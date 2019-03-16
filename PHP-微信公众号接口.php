//index.php用于处理消息output_log.php和output_query.php分别用来输出post过来的数据和请求的查询字符串，（这三个用来调试用的）Utils.php主要用来将数据输出到文件中







//Utils.php，提供了两个函数，traceHttp()将请求的时间、远程主机地址和查询字符串输出到query.xml文件中。logger()将类型、时间和post数据输出到log.xml中。

<?php
class Utils
{
    public static function traceHttp()
    {
        $content = date('Y-m-d H:i:s')."\n\rremote_ip：".$_SERVER["REMOTE_ADDR"].
            "\n\r".$_SERVER["QUERY_STRING"]."\n\r\n\r";
        $max_size = 1000;
        $log_filename = "./query.xml";
        if (file_exists($log_filename) and (abs(filesize($log_filename))) > $max_size){
            unlink($log_filename);
        }else {

        }
        file_put_contents($log_filename, $content, FILE_APPEND);
    }

    public static function logger($log_content, $type = '用户')
    {
        $max_size = 3000;
        $log_filename = "./log.xml";
        if (file_exists($log_filename) and (abs(filesize($log_filename)) >
                $max_size)) {
            unlink($log_filename);
        }
        file_put_contents($log_filename, "$type  ".date('Y-m-d H:i:s')."\n\r".$log_content."\n\r",
            FILE_APPEND);
    }
}



//output_query.php，输出query.xml的内容

<?php
@header('Content-type: text/plain;charset=UTF-8');
$filepath = './query.xml';
readfile($filepath);

//output_log.php，输出log.xml的内容。
<?php
@header('Content-type: text/plain;charset=UTF-8');
$filepath = './log.xml';
readfile($filepath);


//开始写处理消息 index.php

<?php
//设置时区
date_default_timezone_set("Asia/Shanghai");
//定义TOKEN常量，这里的"weixin"就是在公众号里配置的TOKEN
define("TOKEN", "weixin");

require_once("Utils.php");
//打印请求的URL查询字符串到query.xml
Utils::traceHttp();

$wechatObj = new wechatCallBackapiTest();
/**
* 如果有"echostr"字段，说明是一个URL验证请求，
* 否则是微信用户发过来的信息
*/
if (isset($_GET["echostr"])){
    $wechatObj->valid();
}else {
    $wechatObj->responseMsg();
}

class wechatCallBackapiTest
{
    /**
     * 用于微信公众号里填写的URL的验证，
     * 如果合格则直接将"echostr"字段原样返回
     */
    public function valid()
    {
        $echoStr = $_GET["echostr"];
        if ($this->checkSignature()){
            echo $echoStr;
            exit;
        }
    }

    /**
     * 用于验证是否是微信服务器发来的消息
     * @return bool
     */
    private function checkSignature()
    {
        $signature = $_GET["signature"];
        $timestamp = $_GET["timestamp"];
        $nonce = $_GET["nonce"];

        $token = TOKEN;
        $tmpArr = array($token, $timestamp, $nonce);
        sort($tmpArr);
        $tmpStr = implode($tmpArr);
        $tmpStr = sha1($tmpStr);

        if ($tmpStr == $signature){
            return true;
        }else {
            return false;
        }
    }

    /**
     * 响应用户发来的消息
     */
    public function responseMsg()
    {
        //获取post过来的数据，它一个XML格式的数据
        $postStr = $GLOBALS["HTTP_RAW_POST_DATA"];
        //将数据打印到log.xml
        Utils::logger($postStr);
        if (!empty($postStr)){
            //将XML数据解析为一个对象
            $postObj = simplexml_load_string($postStr, 'SimpleXMLElement', LIBXML_NOCDATA);
            $RX_TYPE = trim($postObj->MsgType);
            //消息类型分离
            switch($RX_TYPE){
                case "event":
                    $result = $this->receiveEvent($postObj);
                    break;
                default:
                    $result = "unknow msg type:".$RX_TYPE;
                    break;
            }
            //打印输出的数据到log.xml
            Utils::logger($result, '公众号');
            echo $result;
        }else{
            echo "";
            exit;
        }
    }

    /**
     * 接收事件消息
     */
    private function receiveEvent($object)
    {
        switch ($object->Event){
            //关注公众号事件
            case "subscribe":
                $content = "欢迎关注微微一笑很倾城";
                break;
            default:
                $content = "";
                break;
        }
        $result = $this->transmitText($object, $content);
        return $result;
    }

    /**
     * 回复文本消息
     */
    private function transmitText($object, $content)
    {
        $xmlTpl = "<xml>
    <ToUserName><![CDATA[%s]]></ToUserName>
    <FromUserName><![CDATA[%s]]></FromUserName>
    <CreateTime><![CDATA[%s]]></CreateTime>
    <MsgType><![CDATA[text]]></MsgType>
    <Content><![CDATA[%s]]></Content>
</xml>";
        $result = sprintf($xmlTpl, $object->FromUserName, $object->ToUserName, time(), $content);
        return $result;
    }
}



//微信公众号能够回复用户发送的6种类型消息：文本、图片、语音、视频、音乐、图文（单图文和多图文）。具体格式参见被动回复消息格式                基础接口中事件消息只有：关注和取消关注事件。

//文件结构如下：  images文件夹用来存放图文消息用到的图片，music文件夹用来存放音乐文件；    test.php是点击图文跳转到的链接；index.php用来处理消息。


test.php，这里只输出一个字符串。
<?php
echo "haha";

下面我们直接上index.php的代码，
<?php
//设置时区
date_default_timezone_set("Asia/Shanghai");
//定义TOKEN常量，这里的"weixin"就是在公众号里配置的TOKEN
define("TOKEN", "weixin");

require_once("Utils.php");
//打印请求的URL查询字符串到query.xml
Utils::traceHttp();

$wechatObj = new wechatCallBackapiTest();
/**
 * 如果有"echostr"字段，说明是一个URL验证请求，
 * 否则是微信用户发过来的信息
 */
if (isset($_GET["echostr"])){
    $wechatObj-&gt;valid();
}else {
    $wechatObj->responseMsg();
}

class wechatCallBackapiTest
{
    /**
     * 用于微信公众号里填写的URL的验证，
     * 如果合格则直接将"echostr"字段原样返回
     */
    public function valid()
    {
        $echoStr = $_GET["echostr"];
        if ($this->checkSignature()){
            echo $echoStr;
            exit;
        }
    }

    /**
     * 用于验证是否是微信服务器发来的消息
     * @return bool
     */
    private function checkSignature()
    {
        $signature = $_GET["signature"];
        $timestamp = $_GET["timestamp"];
        $nonce = $_GET["nonce"];

        $token = TOKEN;
        $tmpArr = array($token, $timestamp, $nonce);
        sort($tmpArr);
        $tmpStr = implode($tmpArr);
        $tmpStr = sha1($tmpStr);

        if ($tmpStr == $signature){
            return true;
        }else {
            return false;
        }
    }

    /**
     * 响应用户发来的消息
     */
    public function responseMsg()
    {
        //获取post过来的数据，它一个XML格式的数据
        $postStr = $GLOBALS["HTTP_RAW_POST_DATA"];
        //将数据打印到log.xml
        Utils::logger($postStr);
        if (!empty($postStr)){
            //将XML数据解析为一个对象
            $postObj = simplexml_load_string($postStr, 'SimpleXMLElement', LIBXML_NOCDATA);
            $RX_TYPE = trim($postObj->MsgType);
            //消息类型分离
            switch($RX_TYPE){
                case "event": //事件
                    $result = $this->receiveEvent($postObj);
                    break;
                case "text": //文本
                    $result = $this->receiveText($postObj);
                    break;
                case "image": //图片
                    $result = $this->receiveImage($postObj);
                    break;
                case "voice": //语音
                    $result = $this->receiveVoice($postObj);
                    break;
                case "video": //视频
                    $result = $this->receiveVideo($postObj);
                    break;
                case "location": //地理位置
                    $result = $this->receiveLocation($postObj);
                    break;
                case "link": //链接
                    $result = $this->receiveLink($postObj);
                    break;
                default:
                    $result = "unknow msg type:".$RX_TYPE;
                    break;
            }
            //打印输出的数据到log.xml
            Utils::logger($result, '公众号');
            echo $result;
        }else{
            echo "";
            exit;
        }
    }

    /**
     * 接收文本消息,通过发送指定文本，
     * 让服务器回复相应类型的信息
     */
    private function receiveText($object)
    {
        $keyword = trim($object->Content);
        if (strstr($keyword, "文本")){
            $content = "这是个文本消息";
            //回复文本
            $result = $this->transmitText($object, $content);
        }else if (strstr($keyword, "单图文")){
            $content = array();
            $content[] = array("Title"=>"单图文标题", "Description"=>"单图文内容",
                               "PicUrl"=>"http://weiweiyi.duapp.com/images/img1.jpg", "Url"=>"http://weiweiyi.duapp.com/test.php");
            //回复单图文
            $result = $this->transmitNews($object, $content);
        }else if (strstr($keyword, "多图文")){
            $content = array();
            $content[] = array("Title"=>"多图文1标题", "Description"=>"多图文1描述",
                               "PicUrl"=>"http://weiweiyi.duapp.com/images/img1.jpg", "Url"=>"http://weiweiyi.duapp.com/test.php");
            $content[] = array("Title"=>"多图文2标题", "Description"=>"多图文2描述",
                               "PicUrl"=>"http://weiweiyi.duapp.com/images/img2.jpg", "Url"=>"http://weiweiyi.duapp.com/test.php");
            $content[] = array("Title"=>"多图文3标题", "Description"=>"多图文3描述",
                               "PicUrl"=>"http://weiweiyi.duapp.com/images/img3.jpg", "Url"=>"http://weiweiyi.duapp.com/test.php");
            //回复多图文
            $result = $this->transmitNews($object, $content);
        }else if (strstr($keyword, "音乐")){
            $content = array("Title"=>"好想你", "Description"=>"歌手：朱主爱",
                             "MusicUrl"=>"http://weiweiyi.duapp.com/music/missyou.mp3",
                             "HQMusicUrl"=>"http://weiweiyi.duapp.com/music/missyou.mp3");
            //回复音乐
            $result = $this->transmitMusic($object, $content);
        }else {
            $content = date("Y-m-d", time())."\n".$object->FromUserName."\n-lzc";
            //回复文本
            $result = $this->transmitText($object, $content);
        }
        return $result;
    }

    /**
     * 接收图片消息，通过MediaId回复相同的图片给用户
     */
    private function receiveImage($object)
    {
        $content = array("MediaId"=>$object->MediaId);
        $result = $this->transmitImage($object, $content);
        return $result;
    }

    /*
     * 接收语音消息，通过MediaId回复相同的语音给用户
     */
    private function receiveVoice($object)
    {
        $content = array("MediaId"=>$object->MediaId);
        $result = $this->transmitVoice($object, $content);
        return $result;
    }

    /**
     * 接收视频消息，通过MediaId回复相同的视频给用户，
     * 这个会失败，好像是腾讯的视频需要经过审核才能发送，
     * 避免那种***的视频传播
     */
    private function receiveVideo($object)
    {
        $content = array("MediaId"=>$object->MediaId, "ThumbMediaId"=>$object->ThumbMediaId,
                         "Title"=>"自拍视频", "Description"=>"一个自拍视频");
        $result = $this->transmitVideo($object, $content);
        return $result;
    }

    /**
     * 接收位置消息
     */
    private function receiveLocation($object)
    {
        $content = "你发送的是位置，纬度为：".$object->Location_X."；经度为：".
            $object->Location_Y."；缩放级别为：".$object->Scale."；位置为：".$object->Label;
        $result = $this->transmitText($object, $content);
        return $result;
    }

    /**
     * 接收链接消息
     */
    private function receiveLink($object)
    {
        $content = "你发送的是链接，标题为：".$object->Title."；内容为：".
            $object->Description."；链接地址为：".$object->Url;
        $result = $this->transmitText($object, $content);
        return $result;
    }

    /**
     * 接收事件消息
     */
    private function receiveEvent($object)
    {
        switch ($object->Event){
            case "subscribe": //关注公众号事件
                $content = "欢迎关注微微一笑很倾城";
                break;
            case "unsubscribe": //取消关注事件，取消后用户就收不到消息了，
                $content = ""; //所以发送 ""
                break;
            default:
                $content = "";
                break;
        }
        $result = $this->transmitText($object, $content);
        return $result;
    }

    /**
     * 回复文本消息
     */
    private function transmitText($object, $content)
    {
        $xmlTpl = "<xml>
    <ToUserName><![CDATA[%s]]></ToUserName>
    <FromUserName><![CDATA[%s]]></FromUserName>
    <CreateTime><![CDATA[%s]]></CreateTime>
    <MsgType><![CDATA[text]]></MsgType>
    <Content><![CDATA[%s]]></Content>
</xml>";
        $result = sprintf($xmlTpl, $object->FromUserName, $object->ToUserName, time(), $content);
        return $result;
    }

    /**
     * 回复图文消息
     */
    private function transmitNews($object, $newsArray)
    {
        if (!is_array($newsArray)){
            return;
        }
        $itemTpl = "<item>
    <Title><![CDATA[%s]]></Title>
    <Description><![CDATA[%s]]></Description>
    <PicUrl><![CDATA[%s]]></PicUrl>
    <Url><![CDATA[%s]]></Url>
</item>";
        $item_str = "";
        foreach ($newsArray as $item){
            $item_str .= sprintf($itemTpl, $item["Title"], $item["Description"],
                $item["PicUrl"], $item["Url"]);
        }
        $xmlTpl = "<xml>
    <ToUserName><![CDATA[%s]]></ToUserName>
    <FromUserName><![CDATA[%s]]></FromUserName>
    <CreateTime><![CDATA[%s]]></CreateTime>
    <MsgType><![CDATA[news]]></MsgType>
    <ArticleCount>%s</ArticleCount>
    <Articles>$item_str</Articles>
</xml>";
        $result = sprintf($xmlTpl, $object->FromUserName, $object->ToUserName, time(),
            count($newsArray));
        return $result;
    }

    /**
     * 回复音乐消息
     */
    private function transmitMusic($object, $musicArray)
    {
        $itemTpl = "<Music>
    <Title><![CDATA[%s]]></Title>
    <Description><![CDATA[%s]]></Description>
    <MusicUrl><![CDATA[%s]]></MusicUrl>
    <HQMusicUrl><![CDATA[%s]]></HQMusicUrl>
</Music>";
        $item_str = sprintf($itemTpl, $musicArray["Title"], $musicArray["Description"],
            $musicArray["MusicUrl"], $musicArray["HQMusicUrl"]);
        $xmlTpl = "<xml>
    <ToUserName><![CDATA[%s]]></ToUserName>
    <FromUserName><![CDATA[%s]]></FromUserName>
    <CreateTime>%s</CreateTime>
    <MsgType><![CDATA[music]]></MsgType>
    $item_str;
</xml>";
        $result = sprintf($xmlTpl, $object->FromUserName, $object->ToUserName, time());
        return $result;
    }

    /**
     * 回复图片消息
     */
    private function transmitImage($object, $imageArray)
    {
        $itemTpl = "<Image>
    <MediaId><![CDATA[%s]]></MediaId>
</Image>";

        $item_str = sprintf($itemTpl, $imageArray['MediaId']);

        $textTpl = "<xml>
<ToUserName><![CDATA[%s]]></ToUserName>
<FromUserName><![CDATA[%s]]></FromUserName>
<CreateTime>%s</CreateTime>
<MsgType><![CDATA[image]]></MsgType>
$item_str
</xml>";
        $result = sprintf($textTpl, $object->FromUserName, $object->ToUserName,
            time());
        return $result;
    }

    /**
     * 回复语音消息
     */
    private function transmitVoice($object, $voiceArray)
    {
        $itemTpl = "<Voice>
    <MediaId><![CDATA[%s]]></MediaId>
</Voice>";

        $item_str = sprintf($itemTpl, $voiceArray['MediaId']);

        $textTpl = "<xml>
<ToUserName><![CDATA[%s]]></ToUserName>
<FromUserName><![CDATA[%s]]></FromUserName>
<CreateTime>%s</CreateTime>
<MsgType><![CDATA[voice]]></MsgType>
$item_str
</xml>";

        $result = sprintf($textTpl, $object->FromUserName, $object->ToUserName, time());
        return $result;
    }

    /**
     * 回复视频消息
     */
    private function transmitVideo($object, $videoArray)
    {
        $itemTpl = "<Video>
    <MediaId><![CDATA[%s]]></MediaId>
    <ThumbMediaId><![CDATA[%s]]></ThumbMediaId>
    <Title><![CDATA[%s]]></Title>
    <Description><![CDATA[%s]]></Description>
</Video>";

        $item_str = sprintf($itemTpl, $videoArray['MediaId'], $videoArray['ThumbMediaId'],
            $videoArray['Title'], $videoArray['Description']);

        $textTpl = "<xml>
<ToUserName><![CDATA[%s]]></ToUserName>
<FromUserName><![CDATA[%s]]></FromUserName>
<CreateTime>%s</CreateTime>
<MsgType><![CDATA[video]]></MsgType>
$item_str
</xml>";

        $result = sprintf($textTpl, $object->FromUserName, $object->ToUserName, time());
        return $result;
    }
}

