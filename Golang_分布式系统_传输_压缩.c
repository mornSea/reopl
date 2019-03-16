//Go 语言里创建一个协程非常简单，使用 go 关键词加上一个函数调用就可以了。Go 语言会启动一个新的协程，函数调用将成为这个协程的入口。


package main


import "fmt"

import "time"


func main() {

    fmt.Println("run in main goroutine")

    go func() {

        fmt.Println("run in child goroutine")

        go func() {

            fmt.Println("run in grand child goroutine")

            go func() {

                fmt.Println("run in grand grand child goroutine")

            }()

        }()

    }()

    time.Sleep(time.Second)

    fmt.Println("main goroutine will quit")

}


//保护子协程  因为子协程的异常退出会将异常传播到主协程，直接会导致主协程也跟着挂掉，然后整个程序就崩溃了。
package main


import "fmt"

import "time"


func main() {

    fmt.Println("run in main goroutine")

    go func() {

        fmt.Println("run in child goroutine")

        go func() {

            fmt.Println("run in grand child goroutine")

            go func() {

                fmt.Println("run in grand grand child goroutine")

                panic("wtf")

            }()

        }()

    }()

    time.Sleep(time.Second)

    fmt.Println("main goroutine will quit")

}


在协程的入口函数开头增加 recover() 语句来恢复协程内部发生的异常，阻断它传播到主协程导致程序崩溃。recover 语句必须写在 defer 语句里面。


go func() {

  defer func() {

    if err := recover(); err != nil {

      // log error

    }

  }()

  // do something

}()

//启动百万协程  Go 语言能同时管理上百万的协程，这不是吹牛，下面我们就来编写代码跑一跑这百万协程，读者们请想象一下这百万大军同时奔跑的感觉。  每个协程的内存占用还不到 100 字节。


package main


import "fmt"

import "time"


func main() {

    fmt.Println("run in main goroutine")

    i := 1

    for {

        go func() {

            for {

                time.Sleep(time.Second)

            }

        }()

        if i % 10000 == 0 {

            fmt.Printf("%d goroutine started\n", i)

        }

        i++

    }

}


//设置线程数  默认情况下，Go 运行时会将线程数会被设置为机器 CPU 逻辑核心数。同时它内置的 runtime 包提供了 GOMAXPROCS(n int) 函数允许我们动态调整线程数，注意这个函数名字是全大写，Go 语言的设计者就是这么任性，该函数会返回修改前的线程数，如果参数 n <=0 ，就不会产生修改效果，等价于读操作。


package main


import "fmt"

import "runtime"


func main() {

 // 读取默认的线程数

    fmt.Println(runtime.GOMAXPROCS(0))

    // 设置线程数为 10

 runtime.GOMAXPROCS(10)

    // 读取当前的线程数

 fmt.Println(runtime.GOMAXPROCS(0))

}





//分布式系统：

//功能：能够发送/接收请求和响应  能够连接到集群  如果无法连接到群集（如果它是第一个节点），则可以作为主节点启动节点 每个节点有唯一的标识 能够在节点之间交json数据包  接受命令行参数中的所有信息（将来在我们系统升级时将会很有用）


package main


import (

  "fmt"

  "strconv"

  "time"

  "math/rand"

  "net"

  "flag"

  "strings"

  "encoding/json"

)


// 节点数据信息

type NodeInfo struct {


  // 节点ID，通过随机数生成

  NodeId int `json:"nodeId"`

  // 节点IP地址

  NodeIpAddr string `json:"nodeIpAddr"`

  // 节点端口

  Port string `json: "port"`

}


// 将节点数据信息格式化输出

//NodeInfo:{nodeId: 89423,nodeIpAddr: 127.0.0.1/8,port: 8001}

func (node *NodeInfo) String() string {


  return "NodeInfo:{ nodeId:" + strconv.Itoa(node.NodeId) + ",nodeIpAddr:" + node.NodeIpAddr + ",port:" + node.Port + "}"

}


/* 添加一个节点到集群的一个请求或者响应的标准格式 */

type AddToClusterMessage struct {

  // 源节点

  Source NodeInfo `json:"source"`

  // 目的节点

  Dest NodeInfo `json:"dest"`

  // 两个节点连接时发送的消息

  Message string `json:"message"`

}


/* Request/Response 信息格式化输出 */

func (req AddToClusterMessage) String() string {

  return "AddToClusterMessage:{\n source:" + req.Source.String() + ",\n dest: " + req.Dest.String() + ",\n message:" + req.Message + " }"

}


// cat vi go

// rm


func main() {


  // 解析命令行参数

  makeMasterOnError := flag.Bool("makeMasterOnError", false, "如果IP地址没有连接到集群中，我们将其作为Master节点.")

  clusterip := flag.String("clusterip", "127.0.0.1:8001", "任何的节点连接都连接这个IP")

  myport := flag.String("myport", "8001", "ip address to run this node on. default is 8001.")

  flag.Parse() //解析


  fmt.Println(*makeMasterOnError)

  fmt.Println(*clusterip)

  fmt.Println(*myport)


  /* 为节点生成ID */

  rand.Seed(time.Now().UTC().UnixNano()) //种子

  myid := rand.Intn(99999999) // 随机


  //fmt.Println(myid)


  // 获取IP地址

  myIp,_ := net.InterfaceAddrs()

  fmt.Println(myIp[0])


  // 创建NodeInfo结构体对象

  me := NodeInfo{NodeId: myid, NodeIpAddr: myIp[0].String(), Port: *myport}

  // 输出结构体数据信息

  fmt.Println(me.String())

  dest := NodeInfo{ NodeId: -1, NodeIpAddr: strings.Split(*clusterip, ":")[0], Port: strings.Split(*clusterip, ":")[1]}


  /* 尝试连接到集群，在已连接的情况下并且向集群发送请求 */

  ableToConnect := connectToCluster(me, dest)


  /*

   * 监听其他节点将要加入到集群的请求

   */

  if ableToConnect || (!ableToConnect && *makeMasterOnError) {

    if *makeMasterOnError {fmt.Println("Will start this node as master.")}

    listenOnPort(me)

  } else {

    fmt.Println("Quitting system. Set makeMasterOnError flag to make the node master.", myid)

  }


}


/*

 * 这是发送请求时格式化json包有用的工具

 * 这是非常重要的，如果不经过数据格式化，你最终发送的将是空白消息

 */

func getAddToClusterMessage(source NodeInfo, dest NodeInfo, message string) (AddToClusterMessage){

  return AddToClusterMessage{

    Source: NodeInfo{

      NodeId: source.NodeId,

      NodeIpAddr: source.NodeIpAddr,

      Port: source.Port,

    },

    Dest: NodeInfo{

      NodeId: dest.NodeId,

      NodeIpAddr: dest.NodeIpAddr,

      Port: dest.Port,

    },

    Message: message,

  }

}


func connectToCluster(me NodeInfo, dest NodeInfo) (bool){

  /* 连接到socket的相关细节信息 */

  connOut, err := net.DialTimeout("tcp", dest.NodeIpAddr + ":" + dest.Port, time.Duration(10) * time.Second)

  if err != nil {

    if _, ok := err.(net.Error); ok {

      fmt.Println("未连接到集群.", me.NodeId)

      return false

    }

  } else {

    fmt.Println("连接到集群. 发送消息到节点.")

    text := "Hi nody.. 请添加我到集群.."

    requestMessage := getAddToClusterMessage(me, dest, text)

    json.NewEncoder(connOut).Encode(&requestMessage)


    decoder := json.NewDecoder(connOut)

    var responseMessage AddToClusterMessage

    decoder.Decode(&responseMessage)

    fmt.Println("得到数据响应:\n" + responseMessage.String())


    return true

  }

  return false

}


func listenOnPort(me NodeInfo){

  /* 监听即将到来的消息 */

  ln, _ := net.Listen("tcp", fmt.Sprint(":" + me.Port))

  /* 接受连接 */

  for {

    connIn, err := ln.Accept()

    if err != nil {

      if _, ok := err.(net.Error); ok {

        fmt.Println("Error received while listening.", me.NodeId)

      }

    } else {

      var requestMessage AddToClusterMessage

      json.NewDecoder(connIn).Decode(&requestMessage)

      fmt.Println("Got request:\n" + requestMessage.String())


      text := "Sure buddy.. too easy.."

      responseMessage := getAddToClusterMessage(me, requestMessage.Source, text)

      json.NewEncoder(connIn).Encode(&responseMessage)

      connIn.Close()

    }

  }

}




//传输：

//下载（支持断电续传）（client）

package main

import (

"http"

"os"

"io"

"strconv"

)

const (

UA = "Golang Downloader from Kejibo.com"

)

func main() {

f, err := os.OpenFile("./file.exe", os.O_RDWR, 0666)  

//其实这里的 O_RDWR应该是 O_RDWR|O_CREATE，也就是文件不存在的情况下就建一个空文件，但是因为windows下还有BUG，如果使用这个O_CREATE，就会直接清空文件，所以这里就不用了这个标志，你自己事先建立好文件。

if err != nil { panic(err) }

stat, err := f.Stat()    //获取文件状态

if err != nil { panic(err) }

f.Seek(stat.Size, 0)    //把文件指针指到文件末，当然你说为何不直接用 O_APPEND 模式打开，没错是可以。我这里只是试验。

url := "http://dl.google.com/chrome/install/696.57/chrome_installer.exe"

var req http.Request

req.Method = "GET"

req.UserAgent = UA

req.Close = true

req.URL, err = http.ParseURL(url)

if err != nil { panic(err) }

header := http.Header{}

header.Set("Range", "bytes=" + strconv.Itoa64(stat.Size) + "-")

req.Header = header

resp, err := http.DefaultClient.Do(&req)

if err != nil { panic(err) }

written, err := io.Copy(f, resp.Body)

if err != nil { panic(err) }

println("written: ", written)

}

//下载（server）

package main

import (

"flag"

"fmt"

"io"

"log"

"net/http"

"os"

"path"

"strconv"

)

var dir string

var port int

var staticHandler http.Handler

// 初始化参数

func init() {

dir = path.Dir(os.Args[0])

flag.IntVar(&port, "port", 800, "服务器端口")

flag.Parse()

fmt.Println("dir:", http.Dir(dir))

staticHandler = http.FileServer(http.Dir(dir))

}

func main() {

http.HandleFunc("/", StaticServer)

err := http.ListenAndServe(":"+strconv.Itoa(port), nil)

if err != nil {

log.Fatal("ListenAndServe: ", err)

}

}

// 静态文件处理

func StaticServer(w http.ResponseWriter, req *http.Request) {

fmt.Println("path:" + req.URL.Path)

if req.URL.Path != "/down/" {

staticHandler.ServeHTTP(w, req)

return

}

io.WriteString(w, "hello, world!\n")

}

//上传（client）

package main

import (

"bytes"

"fmt"

"io"

"io/ioutil"

"mime/multipart"

"net/http"

"os"

)

func postFile(filename string, targetUrl string) error {

bodyBuf := &bytes.Buffer{}

bodyWriter := multipart.NewWriter(bodyBuf)

//关键的一步操作

fileWriter, err := bodyWriter.CreateFormFile("uploadfile", filename)

if err != nil {

fmt.Println("error writing to buffer")

return err

}

//打开文件句柄操作

fh, err := os.Open(filename)

if err != nil {

fmt.Println("error opening file")

return err

}

//iocopy

_, err = io.Copy(fileWriter, fh)

if err != nil {

return err

}

contentType := bodyWriter.FormDataContentType()

bodyWriter.Close()

resp, err := http.Post(targetUrl, contentType, bodyBuf)

if err != nil {

return err

}

defer resp.Body.Close()

resp_body, err := ioutil.ReadAll(resp.Body)

if err != nil {

return err

}

fmt.Println(resp.Status)

fmt.Println(string(resp_body))

return nil

}

// sample usage

func main() {

target_url := "http://localhost:9090/upload"

filename := "./astaxie.pdf"

postFile(filename, target_url)

}

//上传（server）

package main

import (

"crypto/md5"

"flag"

"fmt"

"html/template"

"io"

"log"

"net/http"

"os"

"path"

"strconv"

"time"

)

var dir string

var port int

// 初始化参数

func init() {

dir = path.Dir(os.Args[0])

flag.IntVar(&port, "port", 800, "服务器端口")

flag.Parse()

fmt.Println("dir:", http.Dir(dir))

}

func main() {

http.HandleFunc("/upload", upload)

err := http.ListenAndServe(":"+strconv.Itoa(port), nil)

if err != nil {

log.Fatal("ListenAndServe: ", err)

}

}

// 处理/upload 逻辑

func upload(w http.ResponseWriter, r *http.Request) {

fmt.Println("method:", r.Method) //获取请求的方法

if r.Method == "GET" {

crutime := time.Now().Unix()

h := md5.New()

io.WriteString(h, strconv.FormatInt(crutime, 10))

token := fmt.Sprintf("%x", h.Sum(nil))

t, _ := template.ParseFiles("upload.gtpl")

t.Execute(w, token)

} else {

r.ParseMultipartForm(32 << 20)

file, handler, err := r.FormFile("uploadfile")

if err != nil {

fmt.Println(err)

return

}

defer file.Close()

fmt.Fprintf(w, "%v", handler.Header)

f, err := os.OpenFile("./upload/"+handler.Filename, os.O_WRONLY|os.O_CREATE, 0666)

if err != nil {

fmt.Println(err)

return

}

defer f.Close()

io.Copy(f, file)

}

}



//压缩图片：


package main


import (

    //"encoding/json"

    "fmt"

    //"os"

    //"io/ioutil"

    "github.com/nfnt/resize"

    "image/jpeg"

    "io"

    "log"

    "os"

    "path/filepath"

    "strconv"

    // "flag"

    "bufio"

    "image"

    "image/png"

    "strings"

    "time"

)


func imageCompress(

    getReadSizeFile func() (io.Reader, error),

    getDecodeFile func() (*os.File, error),

    to string,

    Quality,

    base int,

    format string) bool {

    /** 读取文件 */

    file_origin, err := getDecodeFile()

    defer file_origin.Close()

    if err != nil {

        fmt.Println("os.Open(file)错误")

        log.Fatal(err)

        return false

    }

    var origin image.Image

    var config image.Config

    var temp io.Reader

    /** 读取尺寸 */

    temp, err = getReadSizeFile()

    if err != nil {

        fmt.Println("os.Open(temp)")

        log.Fatal(err)

        return false

    }

    var typeImage int64

    format = strings.ToLower(format)

    /** jpg 格式 */

    if format == "jpg" || format == "jpeg" {

        typeImage = 1

        origin, err = jpeg.Decode(file_origin)

        if err != nil {

            fmt.Println("jpeg.Decode(file_origin)")

            log.Fatal(err)

            return false

        }

        temp, err = getReadSizeFile()

        if err != nil {

            fmt.Println("os.Open(temp)")

            log.Fatal(err)

            return false

        }

        config, err = jpeg.DecodeConfig(temp)

        if err != nil {

            fmt.Println("jpeg.DecodeConfig(temp)")

            return false

        }

    } else if format == "png" {

        typeImage = 0

        origin, err = png.Decode(file_origin)

        if err != nil {

            fmt.Println("png.Decode(file_origin)")

            log.Fatal(err)

            return false

        }

        temp, err = getReadSizeFile()

        if err != nil {

            fmt.Println("os.Open(temp)")

            log.Fatal(err)

            return false

        }

        config, err = png.DecodeConfig(temp)

        if err != nil {

            fmt.Println("png.DecodeConfig(temp)")

            return false

        }

    }

    /** 做等比缩放 */

    width := uint(base) /** 基准 */

    height := uint(base * config.Height / config.Width)


canvas := resize.Thumbnail(width, height, origin, resize.Lanczos3)

    file_out, err := os.Create(to)

    defer file_out.Close()

    if err != nil {

        log.Fatal(err)

        return false

    }

    if typeImage == 0 {

        err = png.Encode(file_out, canvas)

        if err != nil {

            fmt.Println("压缩图片失败")

            return false

        }

    } else {

        err = jpeg.Encode(file_out, canvas, &jpeg.Options{Quality})

        if err != nil {

            fmt.Println("压缩图片失败")

            return false

        }

    }


return true

}


func getFilelist(path string) {

    /** 创建输出目录 */

    errC := os.MkdirAll(inputArgs.OutputPath, 0777)

    if errC != nil {

        fmt.Printf("%s", errC)

        return

    }

    err := filepath.Walk(path, func(pathFound string, f os.FileInfo, err error) error {

        if f == nil {

            return err

        }

        if f.IsDir() { /** 是否是目录 */

            return nil

        }

        // println(pathFound)

        /** 找到一个文件 */

        /** 判断是不是图片 */

        localPath, format, _ := isPictureFormat(pathFound)

        /** 随机数 */

        t := time.Now()

        millis := t.Nanosecond() /** 纳秒 */

        outputPath := inputArgs.OutputPath + strconv.FormatInt(int64(millis), 10) + "." + format

        if localPath != "" {

            if !imageCompress(

                func() (io.Reader, error) {

                    return os.Open(localPath)

                },

                func() (*os.File, error) {

                    return os.Open(localPath)

                },

                outputPath,

                inputArgs.Quality,

                inputArgs.Width,

                format) {

                fmt.Println("生成缩略图失败")

            } else {

                fmt.Println("生成缩略图成功 " + outputPath)

            }

        }

        return nil

    })

    if err != nil {

        fmt.Printf("输入的路径信息有误 %v\n", err)

    }

}


/** 是否是图片 */

func isPictureFormat(path string) (string, string, string) {

    temp := strings.Split(path, ".")

    if len(temp) <= 1 {

        return "", "", ""

    }

    mapRule := make(map[string]int64)

    mapRule["jpg"] = 1

    mapRule["png"] = 1

    mapRule["jpeg"] = 1

    // fmt.Println(temp[1]+"---")

    /** 添加其他格式 */

    if mapRule[temp[1]] == 1 {

        println(temp[1])

        return path, temp[1], temp[0]

    } else {

        return "", "", ""

    }

}


func execute() {

    /** 获取输入 */

    //str := ""

    //fmt.Scanln (&str) /** 不要使用 scanf，它不会并以一个新行结束输入 */


reader := bufio.NewReader(os.Stdin)

    data, _, _ := reader.ReadLine()

    /** 分割 */

    strPice := strings.Split(string(data), " ") /** 空格 */

    if len(strPice) < 3 {

        fmt.Printf("输入有误，参数数量不足,请重新输入或退出程序：")

        execute()

        return

    }


inputArgs.LocalPath = strPice[0]

    inputArgs.Quality, _ = strconv.Atoi(strPice[1])

    inputArgs.Width, _ = strconv.Atoi(strPice[2])


pathTemp, format, top := isPictureFormat(inputArgs.LocalPath)

    if pathTemp == "" {

        /** 目录 */

        /** 如果输入目录，那么是批量 */

        fmt.Println("开始批量压缩...")

        rs := []rune(inputArgs.LocalPath)

        end := len(rs)

        substr := string(rs[end-1 : end])

        if substr == "/" {

            /** 有 / */

            rs := []rune(inputArgs.LocalPath)

            end := len(rs)

            substr := string(rs[0 : end-1])

            endIndex := strings.LastIndex(substr, "/")

            inputArgs.OutputPath = string(rs[0:endIndex]) + "/LghImageCompress/"

        } else {

            endIndex := strings.LastIndex(inputArgs.LocalPath, "/")

            inputArgs.OutputPath = string(rs[0:endIndex]) + "/LghImageCompress/"

        }

        getFilelist(inputArgs.LocalPath)

        fmt.Println("图片保存在文件夹 " + inputArgs.OutputPath)

    } else {

        /** 单个 */

        /** 如果输入文件，那么是单个，允许自定义路径 */

        fmt.Println("开始单张压缩...") //C:\Users\lzq\Desktop\Apk.jpg 75 200

        inputArgs.OutputPath = top + "_compress." + format

        if !imageCompress(

            func() (io.Reader, error) {

                return os.Open(inputArgs.LocalPath)

            },

            func() (*os.File, error) {

                return os.Open(inputArgs.LocalPath)

            },

            inputArgs.OutputPath,

            inputArgs.Quality,

            inputArgs.Width,

            format) {

            fmt.Println("生成缩略图失败")

        } else {

            fmt.Println("生成缩略图成功 " + inputArgs.OutputPath)

            finish()

        }

    }


time.Sleep(5 * time.Minute) /** 如果不是自己点击退出，延时5分钟 */

}


func finish() {

    fmt.Printf("继续输入进行压缩或者退出程序：")

    execute()

}


func showTips() {

    tips := []string{

        "请输入文件夹或图片路径:",

        "如果输入文件夹,那么该目录的图片将会被批量压缩;",

        "如果是图片路径，那么将会被单独压缩处理。",

        "例如：",

        "C:/Users/lzq/Desktop/headImages/ 75 200",

        "指桌面 headImages 文件夹，里面的图片质量压缩到75%，宽分辨率为200，高是等比例计算",

        "C:/Users/lzq/Desktop/headImages/1.jpg 75 200",

        "指桌面的 headImages 文件夹里面的 1.jpg 图片,质量压缩到75%，宽分辨率为200，高是等比例计算 ",

        "请输入："}

    itemLen := len(tips)

    for i := 0; i < itemLen; i++ {

        if i == itemLen-1 {

            fmt.Printf(tips[i])

        } else {

            fmt.Println(tips[i])

        }

    }

}


type InputArgs struct {

    OutputPath string /** 输出目录 */

    LocalPath string /** 输入的目录或文件路径 */

    Quality int /** 质量 */

    Width int /** 宽度尺寸，像素单位 */

    Format string /** 格式 */

}


var inputArgs InputArgs


func main() {

    showTips()

    execute()

    time.Sleep(5 * time.Minute) /** 如果不是自己点击退出，延时5分钟 */

}









