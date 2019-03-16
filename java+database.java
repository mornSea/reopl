//JDBC,它是java语言中用来规范客户端程序如何来访问数据库的应用程序接口，提供了诸如查询和更新数据库中数据的方法


//零 JDBC驱动加裁注册的代码: 

//Oracle8/8i/9iO数据库(thin模式) 

Class.forName("oracle.jdbc.driver.OracleDriver").newInstance(); 

//Sql Server7.0/2000数据库  
 Class.forName("com.microsoft.jdbc.sqlserver.SQLServerDriver"); 

//Sql Server2005/2008数据库  
 Class.forName("com.microsoft.sqlserver.jdbc.SQLServerDriver"); 

//DB2数据库 

Class.froName("com.ibm.db2.jdbc.app.DB2Driver").newInstance();  

//MySQL数据库 
 Class.forName("com.mysql.jdbc.Driver").newInstance(); 

//一 建立数据库连接，取得Connection对象的不同方式：

 //Oracle8/8i/9i数据库(thin模式) 

  String url="jdbc:oracle:thin:@localhost:1521:orcl"; 

  String user="scott"; 

  String password="tiger"; 

  Connection conn=DriverManager.getConnection(url,user,password); 

  

  //Sql Server7.0/2000/2005/2008数据库 

  String url="jdbc:microsoft:sqlserver://localhost:1433;DatabaseName=pubs"; 

  String user="sa"; 

  String password=""; 

  Connection conn=DriverManager.getConnection(url,user,password); 

  

  //DB2数据库 

  String url="jdbc:db2://localhost:5000/sample"; 

  String user="amdin" 

  String password=-""; 

  Connection conn=DriverManager.getConnection(url,user,password); 

  

//MySQL数据库 

String url="jdbc:mysql://localhost:3306/testDB?user=root&password=root&useUnicode=true&characterEncoding=gb2312"; 

Connection conn=DriverManager.getConnection(url); 

//二 SQL

准备工作要做的就是建立Statement对象PreparedStatement对象，例如：

 //建立Statement对象 

 Statement stmt=conn.createStatement(); 

 //建立PreparedStatement对象 

 String sql="select * from user where userName=? and password=?"; 

  PreparedStatement pstmt=Conn.prepareStatement(sql); 

  pstmt.setString(1,"admin"); 

  pstmt.setString(2,"liubin"); 

做好准备工作之后就可以执行sql语句了，执行sql语句：

String sql="select * from users"; 

ResultSet rs=stmt.executeQuery(sql); 

//执行动态SQL查询 

ResultSet rs=pstmt.executeQuery(); 

//执行insert update delete等语句，先定义sql 

stmt.executeUpdate(sql); 



//三处理结果集 　

　//访问结果记录集ResultSet对象。例如： 

  while(rs.next) 

  {  

  out.println("你的第一个字段内容为："+rs.getString("Name")); 

  out.println("你的第二个字段内容为："+rs.getString(2)); 

  } 

//四 关闭数据库 

 //依次将ResultSet、Statement、PreparedStatement、Connection对象关     闭，释放所占用的资源.例如: 

  rs.close(); 

  stmt.clost(); 

  pstmt.close(); 

  con.close(); 











//五 JDBC与连接池 

//JDBC 事务是用 Connection 对象控制的。JDBC Connection 接口( java.sql.Connection )提供了两种事务模式：自动提交和手工提交。 java.sql.Connection 提供了以下控制事务的方法： 
public void setAutoCommit(boolean) 
public boolean getAutoCommit() 
public void commit() 
public void rollback() 

//使用 JDBC 事务界定时，您可以将多个 SQL 语句结合到一个事务中。JDBC 事务的一个缺点是事务的范围局限于一个数据库连接。一个 JDBC 事务不能跨越多个数据库。

htt

 /**
 ** 数据库连接池类
 **/
package com.chunkyo.db;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Timer;

/**
 * @author chenyanlin
 *
 */
public class DBConnectionPool implements TimerListener {

 private Connection con=null;
 private int inUsed=0;    //使用的连接数
 private ArrayList freeConnections = new ArrayList();//容器，空闲连接
 private int minConn;     //最小连接数
 private int maxConn;     //最大连接
 private String name;     //连接池名字
 private String password; //密码
 private String url;      //数据库连接地址
 private String driver;   //驱动
 private String user;     //用户名
 public Timer timer;      //定时
 /**
  * 
  */
 public DBConnectionPool() {
  // TODO Auto-generated constructor stub
 }
 /**
  * 创建连接池
  * @param driver
  * @param name
  * @param URL
  * @param user
  * @param password
  * @param maxConn
  */
 public DBConnectionPool(String name, String driver,String URL, String user, String password, int maxConn)
 {
  this.name=name;
  this.driver=driver;
  this.url=URL;
  this.user=user;
  this.password=password;
  this.maxConn=maxConn;
 }
 /**
  * 用完，释放连接
  * @param con
  */
 public synchronized void freeConnection(Connection con) 
 {
  this.freeConnections.add(con);//添加到空闲连接的末尾
  this.inUsed--;
 }
 /**
  * timeout  根据timeout得到连接
  * @param timeout
  * @return
  */
 public synchronized Connection getConnection(long timeout)
 {
  Connection con=null;
  if(this.freeConnections.size()>0)
  {
   con=(Connection)this.freeConnections.get(0);
   if(con==null)con=getConnection(timeout); //继续获得连接
  }
  else
  {
   con=newConnection(); //新建连接
  }
  if(this.maxConn==0||this.maxConn<this.inUsed)
  {
   con=null;//达到最大连接数，暂时不能获得连接了。
  }
  if(con!=null)
  {
   this.inUsed++;
  }
  return con;
 }
 /**
  * 
  * 从连接池里得到连接
  * @return
  */
 public synchronized Connection getConnection()
 {
  Connection con=null;
  if(this.freeConnections.size()>0)
  {
   con=(Connection)this.freeConnections.get(0);
   this.freeConnections.remove(0);//如果连接分配出去了，就从空闲连接里删除
   if(con==null)con=getConnection(); //继续获得连接
  }
  else
  {
   con=newConnection(); //新建连接
  }
  if(this.maxConn==0||this.maxConn<this.inUsed)
  {
   con=null;//等待 超过最大连接时
  }
  if(con!=null)
  {
   this.inUsed++;
   System.out.println("得到　"+this.name+"　的连接，现有"+inUsed+"个连接在使用!");
  }
  return con;
 }
 /**
  *释放全部连接
  *
  */
 public synchronized void release()
 {
  Iterator allConns=this.freeConnections.iterator();
  while(allConns.hasNext())
  {
   Connection con=(Connection)allConns.next();
   try
   {
    con.close();
   }
   catch(SQLException e)
   {
    e.printStackTrace();
   }
   
  }
  this.freeConnections.clear();
   
 }
 /**
  * 创建新连接
  * @return
  */
 private Connection newConnection()
 {
  try {
   Class.forName(driver);
   con=DriverManager.getConnection(url, user, password);
  } catch (ClassNotFoundException e) {
   // TODO Auto-generated catch block
   e.printStackTrace();
   System.out.println("sorry can't find db driver!");
  } catch (SQLException e1) {
   // TODO Auto-generated catch block
   e1.printStackTrace();
   System.out.println("sorry can't create Connection!");
  }
  return con;
  
 }
 /**
  * 定时处理函数
  */
 public synchronized void TimerEvent() 
 {
     //暂时还没有实现以后会加上的
 }

 /**
  * @param args
  */
 public static void main(String[] args) {
  // TODO Auto-generated method stub

 }
 /**
  * @return the driver
  */
 public String getDriver() {
  return driver;
 }
 /**
  * @param driver the driver to set
  */
 public void setDriver(String driver) {
  this.driver = driver;
 }
 /**
  * @return the maxConn
  */
 public int getMaxConn() {
  return maxConn;
 }
 /**
  * @param maxConn the maxConn to set
  */
 public void setMaxConn(int maxConn) {
  this.maxConn = maxConn;
 }
 /**
  * @return the minConn
  */
 public int getMinConn() {
  return minConn;
 }
 /**
  * @param minConn the minConn to set
  */
 public void setMinConn(int minConn) {
  this.minConn = minConn;
 }
 /**
  * @return the name
  */
 public String getName() {
  return name;
 }
 /**
  * @param name the name to set
  */
 public void setName(String name) {
  this.name = name;
 }
 /**
  * @return the password
  */
 public String getPassword() {
  return password;
 }
 /**
  * @param password the password to set
  */
 public void setPassword(String password) {
  this.password = password;
 }
 /**
  * @return the url
  */
 public String getUrl() {
  return url;
 }
 /**
  * @param url the url to set
  */
 public void setUrl(String url) {
  this.url = url;
 }
 /**
  * @return the user
  */
 public String getUser() {
  return user;
 }
 /**
  * @param user the user to set
  */
 public void setUser(String user) {
  this.user = user;
 }

}

-------------------------------------------
 DBConnectionManager .java
------------------------------------------
/**
 * 数据库连接池管理类
 */
package com.chunkyo.db;

import java.sql.Connection;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.Properties;
import java.util.Vector;

import com.chunkyo.db.ParseDSConfig;
import com.chunkyo.db.DSConfigBean;
import com.chunkyo.db.DBConnectionPool;

/**
 * @author chenyanlin
 *
 */
public class DBConnectionManager {

 static private DBConnectionManager instance;//唯一数据库连接池管理实例类
 static private int clients;                 //客户连接数
 private Vector drivers  = new Vector();//驱动信息
 private Hashtable pools=new Hashtable();//连接池
 
 /**
  * 实例化管理类
  */
 public DBConnectionManager() {
  // TODO Auto-generated constructor stub
  this.init();
 }
 /**
  * 得到唯一实例管理类
  * @return
  */
 static synchronized public DBConnectionManager getInstance()
 {
  if(instance==null)
  {
   instance=new DBConnectionManager();
  }
  return instance;
  
 }
 /**
  * 释放连接
  * @param name
  * @param con
  */
 public void freeConnection(String name, Connection con)
 {
  DBConnectionPool pool=(DBConnectionPool)pools.get(name);//根据关键名字得到连接池
  if(pool!=null)
  pool.freeConnection(con);//释放连接 
 }
 /**
  * 得到一个连接根据连接池的名字name
  * @param name
  * @return
  */
 public Connection getConnection(String name)
 {
  DBConnectionPool pool=null;
  Connection con=null;
  pool=(DBConnectionPool)pools.get(name);//从名字中获取连接池
  con=pool.getConnection();//从选定的连接池中获得连接
  if(con!=null)
  System.out.println("得到连接。。。");
  return con;
 }
 /**
  * 得到一个连接，根据连接池的名字和等待时间
  * @param name
  * @param time
  * @return
  */
 public Connection getConnection(String name, long timeout)
 {
  DBConnectionPool pool=null;
  Connection con=null;
  pool=(DBConnectionPool)pools.get(name);//从名字中获取连接池
  con=pool.getConnection(timeout);//从选定的连接池中获得连接
  System.out.println("得到连接。。。");
  return con;
 }
 /**
  * 释放所有连接
  */
 public synchronized void release()
 {
  Enumeration allpools=pools.elements();
  while(allpools.hasMoreElements())
  {
   DBConnectionPool pool=(DBConnectionPool)allpools.nextElement();
   if(pool!=null)pool.release();
  }
  pools.clear();
 }

 /**
  * 创建连接池
  * @param props
  */
 private void createPools(DSConfigBean dsb)
 {
  DBConnectionPool dbpool=new DBConnectionPool();
  dbpool.setName(dsb.getName());
  dbpool.setDriver(dsb.getDriver());
  dbpool.setUrl(dsb.getUrl());
  dbpool.setUser(dsb.getUsername());
  dbpool.setPassword(dsb.getPassword());
  dbpool.setMaxConn(dsb.getMaxconn());
  System.out.println("ioio:"+dsb.getMaxconn());
  pools.put(dsb.getName(), dbpool);
 }
 /**
  * 初始化连接池的参数
  */
 private void init()
 {
  //加载驱动程序
  this.loadDrivers();
  //创建连接池
  Iterator alldriver=drivers.iterator();
  while(alldriver.hasNext())
  {
   this.createPools((DSConfigBean)alldriver.next());
   System.out.println("创建连接池。。。");
   
  }
  System.out.println("创建连接池完毕。。。");
 }

 /**
  * 加载驱动程序
  * @param props
  */
 private void loadDrivers()
 {
  ParseDSConfig pd=new ParseDSConfig();
 //读取数据库配置文件
  drivers=pd.readConfigInfo("ds.config.xml");
  System.out.println("加载驱动程序。。。");
 }
 /**
  * @param args
  */
 public static void main(String[] args) {
  // TODO Auto-generated method stub

 }

}
----------------------------------------
DSConfigBean.java
----------------------------------------
/**
 * 配置文件Bean类
 */
package com.chunkyo.db;

/**
 * @author chenyanlin
 *
 */
public class DSConfigBean {

 private String type     =""; //数据库类型
 private String name     =""; //连接池名字
 private String driver   =""; //数据库驱动
 private String url      =""; //数据库url
 private String username =""; //用户名
 private String password =""; //密码
 private int maxconn  =0; //最大连接数
 /**
  * 
  */
 public DSConfigBean() {
  // TODO Auto-generated constructor stub
 }

 /**
  * @param args
  */
 public static void main(String[] args) {
  // TODO Auto-generated method stub

 }

 /**
  * @return the driver
  */
 public String getDriver() {
  return driver;
 }

 /**
  * @param driver the driver to set
  */
 public void setDriver(String driver) {
  this.driver = driver;
 }

 /**
  * @return the maxconn
  */
 public int getMaxconn() {
  return maxconn;
 }

 /**
  * @param maxconn the maxconn to set
  */
 public void setMaxconn(int maxconn) {
  this.maxconn = maxconn;
 }

 /**
  * @return the name
  */
 public String getName() {
  return name;
 }

 /**
  * @param name the name to set
  */
 public void setName(String name) {
  this.name = name;
 }

 /**
  * @return the password
  */
 public String getPassword() {
  return password;
 }

 /**
  * @param password the password to set
  */
 public void setPassword(String password) {
  this.password = password;
 }

 /**
  * @return the type
  */
 public String getType() {
  return type;
 }

 /**
  * @param type the type to set
  */
 public void setType(String type) {
  this.type = type;
 }

 /**
  * @return the url
  */
 public String getUrl() {
  return url;
 }

 /**
  * @param url the url to set
  */
 public void setUrl(String url) {
  this.url = url;
 }

 /**
  * @return the username
  */
 public String getUsername() {
  return username;
 }

 /**
  * @param username the username to set
  */
 public void setUsername(String username) {
  this.username = username;
 }

}
-----------------------------------------------------
ParseDSConfig.java
-----------------------------------------------------
/**
 * 操作配置文件类 读  写 修改 删除等操作 
 */
package com.chunkyo.db;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.List;
import java.util.Vector;
import java.util.Iterator;

import org.jdom.Document;
import org.jdom.Element;
import org.jdom.JDOMException;
import org.jdom.input.SAXBuilder;
import org.jdom.output.Format;
import org.jdom.output.XMLOutputter;

/**
 * @author chenyanlin
 *
 */
public class ParseDSConfig {

 /**
  * 构造函数
  */
 public ParseDSConfig() {
  // TODO Auto-generated constructor stub
 }
 /**
  * 读取xml配置文件
  * @param path
  * @return
  */
 public Vector readConfigInfo(String path)
 {
  String rpath=this.getClass().getResource("").getPath().substring(1)+path;
  Vector dsConfig=null;
  FileInputStream fi = null;
  try
  {
   fi=new FileInputStream(rpath);//读取路径文件
   dsConfig=new Vector();
   SAXBuilder sb=new SAXBuilder();
   Document doc=sb.build(fi);
   Element root=doc.getRootElement();
   List pools=root.getChildren();
   Element pool=null;
   Iterator allPool=pools.iterator();
   while(allPool.hasNext())
   {
    pool=(Element)allPool.next();
    DSConfigBean dscBean=new DSConfigBean();
    dscBean.setType(pool.getChild("type").getText());
    dscBean.setName(pool.getChild("name").getText());
    System.out.println(dscBean.getName());
    dscBean.setDriver(pool.getChild("driver").getText());
    dscBean.setUrl(pool.getChild("url").getText());
    dscBean.setUsername(pool.getChild("username").getText());
    dscBean.setPassword(pool.getChild("password").getText());
    dscBean.setMaxconn(Integer.parseInt(pool.getChild("maxconn").getText()));
    dsConfig.add(dscBean);
   }
   
  } catch (FileNotFoundException e) {
   // TODO Auto-generated catch block
   e.printStackTrace();
  } catch (JDOMException e) {
   // TODO Auto-generated catch block
   e.printStackTrace();
  } catch (IOException e) {
   // TODO Auto-generated catch block
   e.printStackTrace();
  }
  
  finally
  {
   try {
    fi.close();
   } catch (IOException e) {
    // TODO Auto-generated catch block
    e.printStackTrace();
   }
  }
  
  return dsConfig;
 }

/**
 *修改配置文件 没时间写 过段时间再贴上去 其实一样的 
 */
 public void modifyConfigInfo(String path,DSConfigBean dsb) throws Exception
 {
  String rpath=this.getClass().getResource("").getPath().substring(1)+path;
  FileInputStream fi=null; //读出
  FileOutputStream fo=null; //写入
  
 }
/**
 *增加配置文件
 *
 */
 public void addConfigInfo(String path,DSConfigBean dsb) 
 {
  String rpath=this.getClass().getResource("").getPath().substring(1)+path;
  FileInputStream fi=null;
  FileOutputStream fo=null;
  try
  {
   fi=new FileInputStream(rpath);//读取xml流
   
   SAXBuilder sb=new SAXBuilder();
   
   Document doc=sb.build(fi); //得到xml
   Element root=doc.getRootElement();
   List pools=root.getChildren();//得到xml子树
   
   Element newpool=new Element("pool"); //创建新连接池
   
   Element pooltype=new Element("type"); //设置连接池类型
   pooltype.setText(dsb.getType());
   newpool.addContent(pooltype);
   
   Element poolname=new Element("name");//设置连接池名字
   poolname.setText(dsb.getName());
   newpool.addContent(poolname);
   
   Element pooldriver=new Element("driver"); //设置连接池驱动
   pooldriver.addContent(dsb.getDriver());
   newpool.addContent(pooldriver);
   
   Element poolurl=new Element("url");//设置连接池url
   poolurl.setText(dsb.getUrl());
   newpool.addContent(poolurl);
   
   Element poolusername=new Element("username");//设置连接池用户名
   poolusername.setText(dsb.getUsername());
   newpool.addContent(poolusername);
   
   Element poolpassword=new Element("password");//设置连接池密码
   poolpassword.setText(dsb.getPassword());
   newpool.addContent(poolpassword);
   
   Element poolmaxconn=new Element("maxconn");//设置连接池最大连接
   poolmaxconn.setText(String.valueOf(dsb.getMaxconn()));
   newpool.addContent(poolmaxconn);
   pools.add(newpool);//将child添加到root
   Format format = Format.getPrettyFormat();
      format.setIndent("");
      format.setEncoding("utf-8");
      XMLOutputter outp = new XMLOutputter(format);
      fo = new FileOutputStream(rpath);
      outp.output(doc, fo);
  } catch (FileNotFoundException e) {
   // TODO Auto-generated catch block
   e.printStackTrace();
  } catch (JDOMException e) {
   // TODO Auto-generated catch block
   e.printStackTrace();
  } catch (IOException e) {
   // TODO Auto-generated catch block
   e.printStackTrace();
  }
  finally
  {
   
  }
 }
 /**
  *删除配置文件
  */
 public void delConfigInfo(String path,String name)
 {
  String rpath=this.getClass().getResource("").getPath().substring(1)+path;
  FileInputStream fi = null;
  FileOutputStream fo=null;
  try
  {
   fi=new FileInputStream(rpath);//读取路径文件
   SAXBuilder sb=new SAXBuilder();
   Document doc=sb.build(fi);
   Element root=doc.getRootElement();
   List pools=root.getChildren();
   Element pool=null;
   Iterator allPool=pools.iterator();
   while(allPool.hasNext())
   {
    pool=(Element)allPool.next();
    if(pool.getChild("name").getText().equals(name))
    {
     pools.remove(pool);
     break;
    }
   }
   Format format = Format.getPrettyFormat();
      format.setIndent("");
      format.setEncoding("utf-8");
      XMLOutputter outp = new XMLOutputter(format);
      fo = new FileOutputStream(rpath);
      outp.output(doc, fo);

   
  } catch (FileNotFoundException e) {
   // TODO Auto-generated catch block
   e.printStackTrace();
  } catch (JDOMException e) {
   // TODO Auto-generated catch block
   e.printStackTrace();
  } catch (IOException e) {
   // TODO Auto-generated catch block
   e.printStackTrace();
  }
  
  finally
  {
   try {
    fi.close();
   } catch (IOException e) {
    // TODO Auto-generated catch block
    e.printStackTrace();
   }
  }
 }
 /**
  * @param args
  * @throws Exception 
  */
 public static void main(String[] args) throws Exception {
  // TODO Auto-generated method stub

  ParseDSConfig pd=new ParseDSConfig();
  String path="ds.config.xml";
  pd.readConfigInfo(path);
  //pd.delConfigInfo(path, "tj012006");
  DSConfigBean dsb=new DSConfigBean();
  dsb.setType("oracle");
  dsb.setName("yyy004");
  dsb.setDriver("org.oracle.jdbc");
  dsb.setUrl("jdbc:oracle://localhost");
  dsb.setUsername("sa");
  dsb.setPassword("");
  dsb.setMaxconn(1000);
  pd.addConfigInfo(path, dsb);
  pd.delConfigInfo(path, "yyy001");
 }

}

--------------------------------------
ds.config.xml   配置文件
--------------------------------------


<ds-config>
<pool>
<type>mysql</type>
<name>user</name>
<driver>com.mysql.jdbc.driver</driver>
<url>jdbc:mysql://localhost:3306/user</url>
<username>sa</username>
<password>123456</password>
<maxconn>100</maxconn>
</pool>
<pool>
<type>mysql</type>
<name>user2</name>
<driver>com.mysql.jdbc.driver</driver>
<url>jdbc:mysql://localhost:3306/user2</url>
<username>sa</username>
<password>1234</password>
<maxconn>10</maxconn>
</pool>
<pool>
<type>sql2000</type>
<name>books</name>
<driver>com.microsoft.sqlserver.driver</driver>
<url>jdbc:sqlserver://localhost:1433/books:databasename=books</url>
<username>sa</username>
<password></password>
<maxconn>100</maxconn>
</pool>
</ds-config>


3. 连接池的使用
  1。Connection的获得和释放
  DBConnectionManager   connectionMan=DBConnectionManager .getInstance();//得到唯一实例
   //得到连接
   String name="mysql";//从上下文得到你要访问的数据库的名字
   Connection  con=connectionMan.getConnection(name);
  //使用
  。。。。。。。
  // 使用完毕

 connectionMan.freeConnection(name,con);//释放，但并未断开连接





//六 经典代码与实例
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.Driver;
import java.sql.DriverManager;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Enumeration;
import java.util.Vector;
public class ConnectionPool {
private String jdbcDriver = ""; // 数据库驱动
private String dbUrl = ""; // 数据 URL
private String dbUsername = ""; // 数据库用户名
private String dbPassword = ""; // 数据库用户密码
private String testTable = ""; // 测试连接是否可用的测试表名，默认没有测试表
private int initialConnections = 10; // 连接池的初始大小
private int incrementalConnections = 5;// 连接池自动增加的大小
private int maxConnections = 50; // 连接池最大的大小
private Vector connections = null; // 存放连接池中数据库连接的向量 , 初始时为 null

// 它中存放的对象为 PooledConnection 型

/**
* 构造函数
*
* @param jdbcDriver String JDBC 驱动类串
* @param dbUrl String 数据库 URL
* @param dbUsername String 连接数据库用户名
* @param dbPassword String 连接数据库用户的密码
*
*/

public ConnectionPool(String jdbcDriver,String dbUrl,String dbUsername,String dbPassword) {
         this.jdbcDriver = jdbcDriver;
         this.dbUrl = dbUrl;
         this.dbUsername = dbUsername;
         this.dbPassword = dbPassword;
}

/**
* 返回连接池的初始大小
*
* @return 初始连接池中可获得的连接数量
*/
public int getInitialConnections() {

         return this.initialConnections;
}

/**
* 设置连接池的初始大小
*
* @param 用于设置初始连接池中连接的数量
*/

public void setInitialConnections(int initialConnections) {
         this.initialConnections = initialConnections;
}

/**
* 返回连接池自动增加的大小 、
*
* @return 连接池自动增加的大小
*/
public int getIncrementalConnections() {

         return this.incrementalConnections;

}

/**
* 设置连接池自动增加的大小
* @param 连接池自动增加的大小
*/

public void setIncrementalConnections(int incrementalConnections) {

         this.incrementalConnections = incrementalConnections;

}

/**
* 返回连接池中最大的可用连接数量
* @return 连接池中最大的可用连接数量
*/

public int getMaxConnections() {
         return this.maxConnections;
}

/**
* 设置连接池中最大可用的连接数量
*
* @param 设置连接池中最大可用的连接数量值
*/

public void setMaxConnections(int maxConnections) {

         this.maxConnections = maxConnections;

}

/**
* 获取测试数据库表的名字
*
* @return 测试数据库表的名字
*/
public String getTestTable() {

         return this.testTable;

}

/**
* 设置测试表的名字
* @param testTable String 测试表的名字
*/
public void setTestTable(String testTable) {
         this.testTable = testTable;
}

/**
*
* 创建一个数据库连接池，连接池中的可用连接的数量采用类成员
* initialConnections 中设置的值
*/
public synchronized void createPool() throws Exception {

         // 确保连接池没有创建

         // 如果连接池己经创建了，保存连接的向量 connections 不会为空

         if (connections != null) {

          return; // 如果己经创建，则返回

         }

         // 实例化 JDBC Driver 中指定的驱动类实例

         Driver driver = (Driver) (Class.forName(this.jdbcDriver).newInstance());

         DriverManager.registerDriver(driver); // 注册 JDBC 驱动程序

         // 创建保存连接的向量 , 初始时有 0 个元素

         connections = new Vector();

         // 根据 initialConnections 中设置的值，创建连接。

         createConnections(this.initialConnections);

         System.out.println(" 数据库连接池创建成功！ ");

}

/**
* 创建由 numConnections 指定数目的数据库连接 , 并把这些连接
* 放入 connections 向量中
*
* @param numConnections 要创建的数据库连接的数目
*/
@SuppressWarnings("unchecked")
private void createConnections(int numConnections) throws SQLException {

         // 循环创建指定数目的数据库连接

         for (int x = 0; x < numConnections; x++) {

          // 是否连接池中的数据库连接的数量己经达到最大？最大值由类成员 maxConnections

          // 指出，如果 maxConnections 为 0 或负数，表示连接数量没有限制。

          // 如果连接数己经达到最大，即退出。

          if (this.maxConnections > 0 && this.connections.size() >= this.maxConnections) {

           break;

          }

          //add a new PooledConnection object to connections vector

          // 增加一个连接到连接池中（向量 connections 中）

          try{

           connections.addElement(new PooledConnection(newConnection()));

          }catch(SQLException e){

           System.out.println(" 创建数据库连接失败！ "+e.getMessage());

          throw new SQLException();

          }

          System.out.println(" 数据库连接己创建 ......");

         }
}

/**
* 创建一个新的数据库连接并返回它
*
* @return 返回一个新创建的数据库连接
*/
private Connection newConnection() throws SQLException {

         // 创建一个数据库连接

         Connection conn = DriverManager.getConnection(dbUrl, dbUsername, dbPassword);

         // 如果这是第一次创建数据库连接，即检查数据库，获得此数据库允许支持的

         // 最大客户连接数目

         //connections.size()==0 表示目前没有连接己被创建

         if (connections.size() == 0) {

          DatabaseMetaData metaData = conn.getMetaData();

          int driverMaxConnections = metaData.getMaxConnections();

          // 数据库返回的 driverMaxConnections 若为 0 ，表示此数据库没有最大

          // 连接限制，或数据库的最大连接限制不知道

          //driverMaxConnections 为返回的一个整数，表示此数据库允许客户连接的数目

          // 如果连接池中设置的最大连接数量大于数据库允许的连接数目 , 则置连接池的最大

          // 连接数目为数据库允许的最大数目

          if (driverMaxConnections > 0 && this.maxConnections > driverMaxConnections) {

           this.maxConnections = driverMaxConnections;

          }
         }
         return conn; // 返回创建的新的数据库连接

}

/**
* 通过调用 getFreeConnection() 函数返回一个可用的数据库连接 ,
* 如果当前没有可用的数据库连接，并且更多的数据库连接不能创
* 建（如连接池大小的限制），此函数等待一会再尝试获取。
*
* @return 返回一个可用的数据库连接对象
*/

public synchronized Connection getConnection() throws SQLException {

         // 确保连接池己被创建

         if (connections == null) {

          return null; // 连接池还没创建，则返回 null

         }

         Connection conn = getFreeConnection(); // 获得一个可用的数据库连接

         // 如果目前没有可以使用的连接，即所有的连接都在使用中

         while (conn == null){

          // 等一会再试

          wait(250);

          conn = getFreeConnection(); // 重新再试，直到获得可用的连接，如果

          //getFreeConnection() 返回的为 null

          // 则表明创建一批连接后也不可获得可用连接

         }

         return conn;// 返回获得的可用的连接
}

/**
* 本函数从连接池向量 connections 中返回一个可用的的数据库连接，如果
* 当前没有可用的数据库连接，本函数则根据 incrementalConnections 设置
* 的值创建几个数据库连接，并放入连接池中。
* 如果创建后，所有的连接仍都在使用中，则返回 null
* @return 返回一个可用的数据库连接
*/

private Connection getFreeConnection() throws SQLException {

         // 从连接池中获得一个可用的数据库连接

         Connection conn = findFreeConnection();

         if (conn == null) {

          // 如果目前连接池中没有可用的连接

          // 创建一些连接

          createConnections(incrementalConnections);

          // 重新从池中查找是否有可用连接

          conn = findFreeConnection();

          if (conn == null) {

           // 如果创建连接后仍获得不到可用的连接，则返回 null

           return null;

          }

         }

         return conn;

}

/**
* 查找连接池中所有的连接，查找一个可用的数据库连接，
* 如果没有可用的连接，返回 null
*
* @return 返回一个可用的数据库连接
*/

private Connection findFreeConnection() throws SQLException {

         Connection conn = null;

         PooledConnection pConn = null;

         // 获得连接池向量中所有的对象

         Enumeration enumerate = connections.elements();

         // 遍历所有的对象，看是否有可用的连接

         while (enumerate.hasMoreElements()) {

          pConn = (PooledConnection) enumerate.nextElement();

          if (!pConn.isBusy()) {

           // 如果此对象不忙，则获得它的数据库连接并把它设为忙

           conn = pConn.getConnection();

           pConn.setBusy(true);

           // 测试此连接是否可用

           if (!testConnection(conn)) {

            // 如果此连接不可再用了，则创建一个新的连接，

            // 并替换此不可用的连接对象，如果创建失败，返回 null

            try{

             conn = newConnection();

            }catch(SQLException e){

             System.out.println(" 创建数据库连接失败！ "+e.getMessage());

             return null;

            }

            pConn.setConnection(conn);

           }

           break; // 己经找到一个可用的连接，退出

          }

         }

         return conn;// 返回找到到的可用连接

}

/**
* 测试一个连接是否可用，如果不可用，关掉它并返回 false
* 否则可用返回 true
*
* @param conn 需要测试的数据库连接
* @return 返回 true 表示此连接可用， false 表示不可用
*/

private boolean testConnection(Connection conn) {

         try {

          // 判断测试表是否存在

          if (testTable.equals("")) {

           // 如果测试表为空，试着使用此连接的 setAutoCommit() 方法

           // 来判断连接否可用（此方法只在部分数据库可用，如果不可用 ,

           // 抛出异常）。注意：使用测试表的方法更可靠

           conn.setAutoCommit(true);

          } else {// 有测试表的时候使用测试表测试

           //check if this connection is valid

           Statement stmt = conn.createStatement();

           stmt.execute("select count(*) from " + testTable);

          }

         } catch (SQLException e) {

          // 上面抛出异常，此连接己不可用，关闭它，并返回 false;

          closeConnection(conn);

          return false;

         }

         // 连接可用，返回 true

         return true;

}

/**
* 此函数返回一个数据库连接到连接池中，并把此连接置为空闲。
* 所有使用连接池获得的数据库连接均应在不使用此连接时返回它。
*
* @param 需返回到连接池中的连接对象
*/

public void returnConnection(Connection conn) {

         // 确保连接池存在，如果连接没有创建（不存在），直接返回

         if (connections == null) {

          System.out.println(" 连接池不存在，无法返回此连接到连接池中 !");

          return;

         }

         PooledConnection pConn = null;

         Enumeration enumerate = connections.elements();

         // 遍历连接池中的所有连接，找到这个要返回的连接对象

         while (enumerate.hasMoreElements()) {

          pConn = (PooledConnection) enumerate.nextElement();

          // 先找到连接池中的要返回的连接对象

          if (conn == pConn.getConnection()) {

           // 找到了 , 设置此连接为空闲状态

           pConn.setBusy(false);

           break;

          }

         }

}

/**
* 刷新连接池中所有的连接对象
*
*/

public synchronized void refreshConnections() throws SQLException {

         // 确保连接池己创新存在

         if (connections == null) {

          System.out.println(" 连接池不存在，无法刷新 !");

          return;

         }

         PooledConnection pConn = null;

         Enumeration enumerate = connections.elements();

         while (enumerate.hasMoreElements()) {

          // 获得一个连接对象

          pConn = (PooledConnection) enumerate.nextElement();

          // 如果对象忙则等 5 秒 ,5 秒后直接刷新

          if (pConn.isBusy()) {

           wait(5000); // 等 5 秒

          }

          // 关闭此连接，用一个新的连接代替它。

          closeConnection(pConn.getConnection());

          pConn.setConnection(newConnection());

          pConn.setBusy(false);

         }

}

/**
* 关闭连接池中所有的连接，并清空连接池。
*/

public synchronized void closeConnectionPool() throws SQLException {

         // 确保连接池存在，如果不存在，返回

         if (connections == null) {

          System.out.println(" 连接池不存在，无法关闭 !");

          return;

         }

         PooledConnection pConn = null;

         Enumeration enumerate = connections.elements();

         while (enumerate.hasMoreElements()) {

          pConn = (PooledConnection) enumerate.nextElement();

          // 如果忙，等 5 秒

          if (pConn.isBusy()) {

           wait(5000); // 等 5 秒

          }

         //5 秒后直接关闭它

         closeConnection(pConn.getConnection());

         // 从连接池向量中删除它

         connections.removeElement(pConn);

         }

         // 置连接池为空

         connections = null;

}

/**
* 关闭一个数据库连接
*
* @param 需要关闭的数据库连接
*/

private void closeConnection(Connection conn) {

         try {

          conn.close();

         }catch (SQLException e) {

          System.out.println(" 关闭数据库连接出错： "+e.getMessage());

         }

}

/**
* 使程序等待给定的毫秒数
*
* @param 给定的毫秒数
*/

private void wait(int mSeconds) {

         try {

          Thread.sleep(mSeconds);

         } catch (InterruptedException e) {

         }

}

/**
*
* 内部使用的用于保存连接池中连接对象的类
* 此类中有两个成员，一个是数据库的连接，另一个是指示此连接是否
* 正在使用的标志。
*/

class PooledConnection {

         Connection connection = null;// 数据库连接

         boolean busy = false; // 此连接是否正在使用的标志，默认没有正在使用

         // 构造函数，根据一个 Connection 构告一个 PooledConnection 对象

         public PooledConnection(Connection connection) {

          this.connection = connection;

         }

         // 返回此对象中的连接

         public Connection getConnection() {

          return connection;

         }

         // 设置此对象的，连接

         public void setConnection(Connection connection) {

          this.connection = connection;

         }

         // 获得对象连接是否忙

         public boolean isBusy() {

          return busy;

         }

         // 设置对象的连接正在忙

         public void setBusy(boolean busy) {

          this.busy = busy;

         }

}

}

=======================================

这个例子是根据POSTGRESQL数据库写的，
请用的时候根据实际的数据库调整。

调用方法如下：

①　ConnectionPool connPool
                                     = new ConnectionPool("org.postgresql.Driver"
                                                                         ,"jdbc:postgresql://dbURI:5432/DBName"
                                                                         ,"postgre"
                                                                         ,"postgre");

②　connPool .createPool();
　　Connection conn = connPool .getConnection();

(前后端实例：)java连接MySQL数据库 json数据前后端交互

 先在下图文件夹中导入相应的jar包，其中第一个红框中的是使用json数据需导入的7个包，第二个红框是用JDBC连接MySQL数据库必须的包。

连接MySQL的工具类：

package com.XXXXXX.util;

import java.sql.Connection;
import java.sql.DriverManager;

public class MysqlUtil
{
    private static Connection conn = null; //数据库连接
    private static String server = "localhost"; //服务ip
    private static String port = "3306"; //服务端口
    public static String dbname = "XXXXX"; //数据库名
    private static String user = "root"; //数据库用户名
    private static String pass = "XXXXX"; //数据库密码
    private static String drivername = "com.mysql.jdbc.Driver"; //MySql驱动类名
    private static String url = "jdbc:mysql://" + server + ":" + port + "/" + dbname + "?user=" + user + "&password=" + pass + "&useUnicode=true&characterEncoding=UTF-8"; //连接地址

    public static Connection getConn()
    {
        try
        {
            Class.forName(drivername).newInstance(); //加载JDBC驱动
            conn = DriverManager.getConnection(url); //建立连接
        } catch (Exception e)
        {
            e.printStackTrace();
        }
        return conn;
    }
}

服务端上的查询Servlet：

package com.XXXXX.servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

import com.webmappoi.util.MysqlUtil;

public class GetPoiInfoServlet extends HttpServlet
{

    /**
     *
     */
    private static final long serialVersionUID = 1L;

    /**
     * The doGet method of the servlet. <br>
     *
     * This method is called when a form has its tag value method equals to get.
     *
     * @param request
     *            the request send by the client to the server
     * @param response
     *            the response send by the server to the client
     * @throws ServletException
     *             if an error occurred
     * @throws IOException
     *             if an error occurred
     */
    public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {

        /*
         * response.setContentType("text/html"); PrintWriter out =
         * response.getWriter();out.println(
         * "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">");
         * out.println("<HTML>");
         * out.println("  <HEAD><TITLE>A Servlet</TITLE></HEAD>");
         * out.println("  <BODY>"); out.print("    This is ");
         * out.print(this.getClass()); out.println(", using the GET method");
         * out.println("  </BODY>"); out.println("</HTML>"); out.flush();
         * out.close();
         */

        this.doPost(request, response);
    }

    /**
     * The doPost method of the servlet. <br>
     *
     * This method is called when a form has its tag value method equals to
     * post.
     *
     * @param request
     *            the request send by the client to the server
     * @param response
     *            the response send by the server to the client
     * @throws ServletException
     *             if an error occurred
     * @throws IOException
     *             if an error occurred
     */
    public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {

        response.setContentType("text/html;charset=UTF-8");
        PrintWriter out = response.getWriter();

        JSONArray array = new JSONArray();
        Connection con = null;
        PreparedStatement ps = null;
        ResultSet rs = null;
        String sql = "select * from university";
        try
        {
            con = MysqlUtil.getConn();
            ps = con.prepareStatement(sql);
            rs = ps.executeQuery();
            while (rs.next())
            {
                JSONObject temp = new JSONObject().element("id", rs.getInt("U_ID")).element("point", rs.getString("U_Point")).element("title", rs.getString("U_Title")).element("address", rs.getString("U_Addr")).element("tel", rs.getString("U_Tel"));
                array.add(temp);
            }
            out.print(array.toString()); //将array对象传到前端
            System.out.println(array.toString());
            out.flush();
            out.close();
        } catch (SQLException e)
        {
            e.printStackTrace();
        } finally
        {
            try
            {
                //依次关闭连接
                if (rs != null)
                {
                    rs.close();
                    rs = null;
                }
                if (ps != null)
                {
                    ps.close();
                    ps = null;
                }
                if (con != null)
                {
                    con.close();
                    con = null;
                }
            } catch (SQLException e)
            {
                e.printStackTrace();
            }
        }
    }
}

前端js接收array对象并解析：

$.getJSON("./servlet/GetPoiInfoServlet",function(json)
{
for(var i=0;i<json.length;i++)
        {
        //var json = eval(json);
            //console.log(JSON.stringify(json));
            //获取经纬度
            ulng = parseFloat(json[i].point.split(",")[0]); //以,分割成数组
            ulat = parseFloat(json[i].point.split(",")[1]);
            var uid = parseFloat(json[i].id);
            var title = json[i].title;
            var address = json[i].address;
            var point = json[i].point;
            var tel = json[i].tel;
        }
});

/********************************************************************

getJSON函数所接收的数据就是json格式，所以可以不用eval函数转化。function中参数json就是后台Servlet传来的array对象。getJSON函数还有一个问题就是，用它从前端传数据到后台时，如果有中文的话，传到后台的数据就会乱码。（网上有解决办法，但我没试过，我就用post方法将就了，post方法不会乱码，但是在接收是需要用eval函数转化。）
********************************************************************/

前端js中封装表单成json数据：

function GetJsonData()
{
    var json =
    {
            "point": GID("point").value,
            "title": GID("title").value,
            "address": $("#address").val(),
            "tel": $("#tel").val()
        };
        return json;
}

前端js中封装document.getElementById方法：

function GID(id)
{
    return document.getElementById(id);
}

前端js中用Jquery post方法提交表单json数据：

function post()
{
/*$("#status").html("正在提交数据，请勿关闭当前窗口...");
    var url = GID("form1").action;
        //alert(JSON.stringify(GetJsonData()));
        $.ajax(
        {
            type: "POST",
            //url: "./servlet/AddPoiDataServlet",
            url: url,
            //contentType: "application/json; charset=utf-8",
            data: {data: JSON.stringify(GetJsonData())},
            //dataType: "json",
            success: function (json)
            {
                if (json.length > 0)
                {
                    alert("提交成功！");
                }
            },
            error: function (json)
            {
                $("#status").html("提交数据失败！");
            }
        });*/

    var url = GID("form1").action;
    //alert(url);
    $("#status").html("正在提交数据，请勿关闭当前窗口...");
    $.post(url,{data:JSON.stringify(GetJsonData())},function (msg)
{
    //alert("提交成功！");
            $("#status").html("提交成功！");
     });
}

/*******************************************************************************

JSON.stringify()函数是为了将json数据字符串化。Jquery ajax和post方法都能将表单传到后端，post方法封装了ajax，用起来更简单。
******************************************************************************/

后台Servlet接收前端传来的json数据并保存到MySQL数据库中：

//System.out.println("地名:" + request.getParameter("title") + "<br>");
        JSONObject json = JSONObject.fromObject(request.getParameter("data")); //获取前端传来的data
        System.out.println(json.toString());
        Connection con = null;
        PreparedStatement ps = null;
        int result = 0;
        String sql = "insert into university(U_Point,U_Title,U_Addr,U_Tel) values(?,?,?,?)";
        try
        {
            con = MysqlUtil.getConn();
            ps = con.prepareStatement(sql);
            ps.setString(1, json.getString("point"));
            ps.setString(2, json.getString("title"));
            ps.setString(3, json.getString("address"));
            ps.setString(4, json.getString("tel"));
            result = ps.executeUpdate();
            out.print(json.toString());
            System.out.println(json.toString());
            out.flush();
            out.close();
        } catch (SQLException e)
        {
            e.printStackTrace();
        } finally
        {
            try
            {
                if (ps != null)
                {
                    ps.close();
                    ps = null;
                }
                if (con != null)
                    con.close();
                    con = null;
            } catch (SQLException e)
            {
                e.printStackTrace();
            }
        }

其它的修改，删除Servlet：
删除servlet：

String sql = "delete from university where U_ID=?";
try
{
    con = MysqlUtil.getConn();
    ps = con.prepareStatement(sql);
    ps.setInt(1, json.getInt("id"));
    result = ps.executeUpdate();
    json.put("result", true);
    out.print(json.toString());
    System.out.println(json.toString());
    out.flush();
    out.close();
}

修改servlet：

String sql = "update university set U_Title=?,U_Addr=?,U_Tel=? where U_Point=?";
try
{
    con = MysqlUtil.getConn();
    ps = con.prepareStatement(sql);
    ps.setString(1, json.getString("title"));
    ps.setString(2, json.getString("address"));
    ps.setString(3, json.getString("tel"));
    ps.setString(4, json.getString("point"));
    result = ps.executeUpdate();
    //json.put("result", true);
    out.print(json.toString());
    System.out.println(json.toString());
    out.flush();
    out.close();
}

模糊查询：
前端js传输查询条件并接受后台查询数据库之后传过来的json数据：

function selectInfo()  //模糊查询
{
    var json =
    {
        "title": GID("info").value
    };
    //alert(JSON.stringify(json));
    $.post("./servlet/SelectPoiInfoServlet",{data:JSON.stringify(json)},function(json)
    {
         for(var i=0;i<json.length;i++)
         {
            var json = eval(json);
             //获取经纬度
             ulng = parseFloat(json[i].point.split(",")[0]);
             ulat = parseFloat(json[i].point.split(",")[1]);

             var uid = parseFloat(json[i].id);
             var title = json[i].title;
             var address = json[i].address;
             var point = json[i].point;
             var tel = json[i].tel;
           }
    });
}

/*****************************************************************************************

这里不能用getJSON方法，因为如果前端传过来json数据有中文，到后台就会乱码，所以就用post方法。如果这里传的不是json数组，而仅仅是一个json对象，就需要将eval函数改为var json = eval("(" + json + ")");，不然前端会报错：SyntaxError: missing ; before statement。
******************************************************************************************/

后台模糊查询的servlet：

public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException
    {

        response.setContentType("text/html;charset=UTF-8");
        PrintWriter out = response.getWriter();

        JSONObject json = JSONObject.fromObject(request.getParameter("data"));
        System.out.println(json.toString());
        JSONArray array = new JSONArray();
        Connection con = null;
        PreparedStatement ps = null;
        ResultSet rs = null;
        String sql = "select * from university where U_Title like ?";
        try
        {
            con = MysqlUtil.getConn();
            ps = con.prepareStatement(sql);
            ps.setString(1, "%" + json.getString("title") + "%");
            rs = ps.executeQuery();
            while (rs.next())
            {
                JSONObject temp = new JSONObject().element("id", rs.getInt("U_ID")).element("point", rs.getString("U_Point")).element("title", rs.getString("U_Title")).element("address", rs.getString("U_Addr")).element("tel", rs.getString("U_Tel"));
                array.add(temp);
            }
            out.print(array.toString()); //将array对象传到前端
            System.out.println(array.toString());
            out.flush();
            out.close();
        }

关于模糊查询：
参考JDBC 模糊查询 传参问题，附文如下：
错误写法：String sql = “select * from tableName where column like ‘%?%’”;
这样写，？是不能传参进来的，因为编译的时候？没被编译成传参标识，所以执行时会抛出SQLException。
正确写法有两种：
1、String sql = “select * from tableName where column like ‘%” + columnValue + “%’”; //可以直接将变量值写进sql语句
2、String sql = “select * from tableName where column like ?”; //可以利用jdbc的传参函数传进
   PreparedStatement ps = conn.PreparedStatement(sql);
   ps.setString(1, “%” + columnValue + “%”);