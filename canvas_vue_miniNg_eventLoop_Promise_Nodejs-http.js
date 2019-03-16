//粒子动画
//先创建一个canvas画布：

// 这里就简单地设置下背景色
<body style="background:#f7fafc;">
  <canvas id="canvas" style="width: 100%; height: 100%;"></canvas>
</body>
//接着先获取canvas的上下文环境并设置一些共用的属性

var canvas = document.getElementById("canvas");
var context = canvas.getContext("2d");

canvas.width = document.documentElement.clientWidth;
canvas.height = document.documentElement.clientHeight;

context.fillStyle = "rgba(0, 0, 0, 0.08)";
context.strokeStyle = "rgba(0, 0, 0, 0.05)";
context.lineWidth = 0.5;


//接下来绘制圆，那么绘制圆需要圆的圆心坐标，半径，水平方向的速度，垂直方向的速度，并且这些信息要满足一定的条件，通过一个函数来创建：

// 存放所有圆的数组，这里用balls
var balls = [];
function createBall() {
  // x坐标
  var _x = Math.random() * canvas.width;
  // y坐标
  var _y = Math.random() * canvas.height;
  // 半径 [0.01, 15.01]
  var _r = Math.random() * 15 + 0.01;
  // 水平速度 [±0.0, ±0.5]
  var _vx = Math.random() * 0.5 * Math.pow( -1, Math.floor(Math.random() * 2 + 1) );
  // 垂直速度 [±0.0, ±0.5]
  var _vy = Math.random() * 0.5 * Math.pow( -1, Math.floor(Math.random() * 2 + 1) );

    // 把每一个圆的信息存放到数组中
  balls.push({
    x: _x,
    y: _y,
    r: _r,
    vx: _vx,
    vy: _vy
  });
}

//然后根据自己的情况选择需要绘制多少个圆，这里我假设有20个，看起来舒服一点：

// 圆的数量
var num = 20;
for(var i = 0; i < num; i++) {
  createBall();
}

//现在圆的信息都有了，下一步就是绘制每一帧的圆和线，创建一个render函数，然后在函数内先绘制所有的圆出来：

for(var k = 0; k < num; k++) {
  context.save();
  context.beginPath();
  context.arc( balls[k].x, balls[k].y, balls[k].r, 0, Math.PI*2 );
  context.fill();
  context.restore();
}

//接着要遍历每两个圆的圆心之间的距离是否小于某个临界值（比如500），满足则将这两个圆的圆心连接起来：

for(var i = 0; i < num; i++) {
  for(var j = i + 1; j < num; j++) {
    if( distance( balls[i], balls[j] ) < 500 ) {
      context.beginPath();
      context.moveTo( balls[i].x, balls[i].y );
      context.lineTo( balls[j].x, balls[j].y );
      context.stroke();
    }
  }
}

//这里的 distance 函数就是计算两点之间的距离：

function distance(point1, point2) {
  return Math.sqrt( Math.pow( (point1.x - point2.x), 2 ) + Math.pow( (point1.y - point2.y), 2 ) );
}

//还有一步就是判断圆是否超出了边界值，若满足条件则从对边再次进来：

for(var k = 0; k < num; k++) {
  balls[k].x += balls[k].vx;
  balls[k].y += balls[k].vy;

  if( balls[k].x - balls[k].r > canvas.width ) {
    balls[k].x = 0 - balls[k].r;
  }
  if( balls[k].x + balls[k].r < 0 ) {
    balls[k].x = canvas.width + balls[k].r;
  }
  if( balls[k].y - balls[k].r > canvas.height ) {
    balls[k].y = 0 - balls[k].r;
  }
  if( balls[k].y + balls[k].r < 0 ) {
    balls[k].y = canvas.height + balls[k].r;
  }
}

//当然如果想简单点，只要圆超出就移除并重新生成一个圆即可：

if( balls[k].x - balls[k].r > canvas.width || 
    balls[k].x + balls[k].r < 0 || 
    balls[k].y - balls[k].r > canvas.height || 
    balls[k].y + balls[k].r < 0) {
  balls.splice(k, 1);
  createBall();
}

//这样每一帧绘制的细节就完成了，最后一步就是让圆都运动起来：

(function loop(){
  render();
  requestAnimationFrame(loop);
})();




//vue
//新挂载$mount方法
Vue.prototype.$mount = function (
  el?: string | Element,
  hydrating?: boolean
): Component {
  el = el && query(el)
  .....
}

编译RenderFn

el = el && query(el)对传入的el元素节点做了确认，如果传入的节点容器没有找到的便警告并且return一个createElement('div')新的div。

//判断传入的标签如果是body或者是页面根节点
//就警告禁止挂载在页面根节点上，因为挂载会替换该节点。最后返回该节点
  if (el === document.body || el === document.documentElement) {
    process.env.NODE_ENV !== 'production' && warn(
      `Do not mount Vue to <html> or <body> - mount to normal elements instead.`
    )
    return this
  }
    
  const options = this.$options;    
  if (!options.render) {    //如果接受的值已经有写好的RenderFn，则不用进行任何操作,如果render不存在，就进入此逻辑将模板编译成renderFn
    let template = options.template
    if (template) {
        ...  //有template就使用idToTemplate()解析,最终返回该节点的innerHTML
      } if (typeof template === 'string') {
        if (template.charAt(0) === '#') {//如果模板取到的第一个字符是#
          template = idToTemplate(template)
          if (process.env.NODE_ENV !== 'production' && !template) {//开发环境并且解析模板失败的报错：警告模板为空或者未找到
            warn(
              `Template element not found or is empty: ${options.template}`,
              this
            )
          }
        }
      }else if (template.nodeType) {
        //如果有节点类型，判定是普通节点，也返回innerHTML
        template = template.innerHTML  
      } else {  
        //没有template就警告该模板无效
        if (process.env.NODE_ENV !== 'production') {
          warn('invalid template option:' + template, this)
        }
        return this
      }
    } else if (el) {
        //如果是节点的话，获取html模板片段,getOuterHTML()对传入的el元素做了兼容处理，最终目的是拿到节点的outerHTML
        //getOuterHTML()可以传入DOM节点，CSS选择器，HTML片段
      template = getOuterHTML(el)
    }
    if (template) {
    //编译HTML生成renderFn，赋给options,vm.$options.render此时发生变化
      if (process.env.NODE_ENV !== 'production' && config.performance && mark) {
        //开始标记
        mark('compile')
      }
      /*  compileToFunctions()主要是将getOuterHTML获取的模板编译成RenderFn函数，该函数的具体请往后翻看
      *  具体步骤之后再说，编译大致主要分成三步 
      *  1.parse：将 html 模板解析成抽象语法树(AST)。
      *  2.optimizer：对 AST 做优化处理。
      *  3.generateCode：根据 AST 生成 render 函数。
      */ 
      const { render, staticRenderFns } = compileToFunctions(template, {
        shouldDecodeNewlines,
        shouldDecodeNewlinesForHref,
        delimiters: options.delimiters,
        comments: options.comments
      }, this)
      options.render = render;  //最后将解析的renderFn 赋值给当前实例
      options.staticRenderFns = staticRenderFns //编译的配置
      
      if (process.env.NODE_ENV !== 'production' && config.performance && mark) {
        //结束标记
        mark('compile end')
        //根据mark()编译过程计算耗时差，用于到控制台performance查看阶段渲染性能
        measure(`vue ${this._name} compile`, 'compile', 'compile end')
      }
    }
  }
    //最后返回之前储存的mount()方法进行挂载，如果此前renderFn存在就直接进行此步骤
    return mount.call(this, el, hydrating)
}



//__patch__是整个整个virtaul-dom当中最为核心的方法了，主要功能是对prevVnode(旧vnode)和新vnode进行diff的过程，经过patch比对，最后生成新的真实dom节点更新改变部分的视图。

return function patch (oldVnode, vnode, hydrating, removeOnly, parentElm, refElm) {
    //用到的参数，oldVnode：旧的vnode、vnode：新的vnode、hydrating：服务端渲染、removeOnly：避免误操作
    //当新的vnode不存在，并且旧的vnode存在时，直接返回旧的vnode，不做patch
    if (isUndef(vnode)) {
      if (isDef(oldVnode)) { invokeDestroyHook(oldVnode); }
      return
    }
    var insertedVnodeQueue = [];

    //如果旧的vnode不存在
    if (isUndef(oldVnode)) {
      //就创建一个新的节点
      createElm(vnode, insertedVnodeQueue, parentElm, refElm);
    } else {
      //获取旧vnode的节点类型
      var isRealElement = isDef(oldVnode.nodeType);
      // 如果不是真实的dom节点并且属性相同
      if (!isRealElement && sameVnode(oldVnode, vnode)) {
        // 对oldVnode和vnode进行diff，并对oldVnode打patch
        patchVnode(oldVnode, vnode, insertedVnodeQueue, removeOnly);
      } 
      }
    }
    //最后返回新vnode的节点内容
    return vnode.elm
  }


//new Vue():
//创建Vue函数，并且检测当前是不是开发环境，如果Vue不是通过new实例化的将警告。然后初始化this._init(options)。为什么(this instanceof Vue)这一句可以判断是否使用了new操作符？

//已new来调用构造函数会经历4个步骤：
//- 创建一个新对象；
//- 将构造函数的作用域赋给新对象（因此this 就指向了这个新对象）；
//- 执行构造函数中的代码（为这个新对象添加属性）；
//- 返回新对象。 而instanceof用来检测Vue构造函数的prototype是否存在于this的原型链上，换句话说，如果使用new实例化的时候，this就指向了这个新创建的对象，这时this instanceof Vue这句话的意思就是判断新创建的对象是否是Vue类型的，也就相当于判断新实例对象的constructor是否是Vue构造函数。



//双向数据绑定

//在平常，我们很容易就可以打印出一个对象的属性数据：

var Book = {
  name: 'vue权威指南'
  };
console.log(Book.name);  // vue权威指南

//如果想要在执行console.log(book.name)的同时，直接给书名加个书名号，那要怎么处理呢？或者说要通过什么监听对象 Book 的属性值。这时候Object.defineProperty( )就派上用场了，代码如下：

var Book = {}
var name = '';
Object.defineProperty(
    Book, 'name', {
      set: function (value) { 
       name = value;
           console.log(
           '你取了一个书名叫做' + value
          );  },
          
            get: function () {
              return '《' + name + '》'
  }}) 
Book.name = 'vue权威指南';  
// 你取了一个书名叫做vue权威指南
console.log(Book.name);  // 《vue权威指南》

//我们通过Object.defineProperty( )设置了对象Book的name属性，对其get和set进行重写操作，顾名思义，get就是在读取name属性这个值触发的函数，set就是在设置name属性这个值触发的函数，所以当执行 Book.name = 'vue权威指南' 这个语句时，控制台会打印出 "你取了一个书名叫做vue权威指南"，紧接着，当读取这个属性时，就会输出 "《vue权威指南》"，因为我们在get函数里面对该值做了加工了。

//要实现mvvm的双向绑定，就必须要实现以下几点：
//1、实现一个数据监听器Observer，能够对数据对象的所有属性进行监听，如有变动可拿到最新值并通知订阅者
//2、实现一个指令解析器Compile，对每个元素节点的指令进行扫描和解析，根据指令模板替换数据，以及绑定相应的更新函数
//3、实现一个Watcher，作为连接Observer和Compile的桥梁，能够订阅并收到每个属性变动的通知，执行指令绑定的相应回调函数，从而更新视图
//4、mvvm入口函数，整合以上三者

//前端浏览器中，有些数据（比如数据字典中的数据），可以在第一次请求的时候全部拿过来保存在js对象中，以后需要的时候就不用每次都去请求服务器了。对于那些大量使用数据字典来填充下拉框的页面，这种方法可以极大地减少对服务器的访问。这种方法特别适用于使用iframe的框架。

//具体实现思路和方法：创建一个cache.js文件：

//1、前端页面，定义那些数据需要一次性拿到前端缓存，定义一个对象来保存这些数据：

/**
 * 定义需要在用户登录的时候获取到本地的数据字典类别
 */
var clsCodes = {clsCodes :
    [BOOL,
     STATUS,
     USER_TYPE,
     REPORT_STATUS
   ]
};

/**
 * 获取数据字典到本地
 */
var dicts;

//2、前端页面，定义一个函数来调用后台接口获取数据，然后保存到本地缓存对象（dicts）中。

function getDicts() {
  $.post(getContextPath() + /api/sys/getDictList,
      clsCodes,
      function(resultBean, status, xhRequest) {
        if (resultBean.data != undefined) {
          dicts = resultBean.data;
        }
      },
      'json');
}
/-在主页面加载的时候调用这个方法一次性获取数据并缓存起来。这样，以后需要同样的数据，就直接从本地对象dicts中获取了。

//后端Controller：
//3、定义一个接口，根据前端的请求，查询数据库（或查询服务器缓存，如下面例子中）获取数据返回给前端：

/**
 * 根据多个分类编号获取多个字典集合
 * @param clsCodes
 * @return {{clsCode : {code1:name1,code2:name2...}}, ...}
 */
@SuppressWarnings({ unchecked, rawtypes })
@ResponseBody
@RequestMapping(getDictList)
public ResultBean getDictList(@RequestParam(value = clsCodes[], required = true) String[] clsCodes) {
  ResultBean rb = new ResultBean();

  Map<string, string="">> dictCache = (Map<string, string="">>) CacheManager.getInstance().get(CacheConstants.DICT);
  Map dictMap = new LinkedHashMap<>(); //使用LinkedHashMap保证顺序

  if(dictCache != null){
    for(String clsCode: clsCodes){
      dictMap.put(clsCode, dictCache.get(clsCode));
    }
  }else{
    rb.setMessage(缓存中拿不到字典信息！);
    rb.setSuccess(false);
  }

  rb.setData(dictMap);
  return rb;
}</string,></string,>






//200行代码创建属于你自己的精简版angular:

//会实现的angular的组件包括：  Controllers  Directives  Services
//为了达到这些功能我们需要实现$compileservice(我们称之为DOMCompiler),还有$provider跟$injector(在我们的实现里统称为Provider)。为了实现双向绑定我们还要实现scope。

var Provider = {
  _providers: {},
  directive: function (name, fn) {
    this._register(name + Provider.DIRECTIVES_SUFFIX, fn);
  },
  controller: function (name, fn) {
    this._register(name + Provider.CONTROLLERS_SUFFIX, function () {
      return fn;
    });
  },
  service: function (name, fn) {
    this._register(name, fn);
  },
  _register: function (name, factory) {
    this._providers[name] = factory;
  }
  //...
};
Provider.DIRECTIVES_SUFFIX = 'Directive';
Provider.CONTROLLERS_SUFFIX = 'Controller';

//上面的代码提供了一个针对注册组件的简单的实现。我们定义了一个私有属性_provides用来存储所有的组件的工厂函数。我们还定义了directive,service和controller这些方法。这些方法本质上内部会调用_register来实现。在controller方法里面我们简单的在给的工厂函数外面包装了一层函数，因为我们希望可以多次实例化同一个controller而不去缓存返回的值。

var Provider = {
  // ...
  get: function (name, locals) {
    if (this._cache[name]) {
      return this._cache[name];
    }
    var provider = this._providers[name];
    if (!provider || typeof provider !== 'function') {
      return null;
    }
    return (this._cache[name] = this.invoke(provider, locals));
  },
  annotate: function (fn) {
    var res = fn.toString()
        .replace(/((\/\/.*$)|(\/\*[\s\S]*?\*\/))/mg, '')
        .match(/\((.*?)\)/);
    if (res && res[1]) {
      return res[1].split(',').map(function (d) {
        return d.trim();
      });
    }
    return [];
  },
  invoke: function (fn, locals) {
    locals = locals || {};
    var deps = this.annotate(fn).map(function (s) {
      return locals[s] || this.get(s, locals);
    }, this);
    return fn.apply(null, deps);
  },
  _cache: { $rootScope: new Scope() }
};
此博客不再维护，博客已迁移至 https://github.com/purplebamboo/blog/issues blog of purplebamboo
blog of purplebamboo
dota诚可贵，coding价更高，若为女朋友，二者皆可抛。
首页
归档
分类
关于

搜索
(翻译)使用200行代码创建属于你自己的精简版angular
By purplebamboo

 5月 27 2015 更新日期:6月 21 2015

文章目录
1. 主要的组件：
1.1. Provider
1.2. DOMCompiler
1.3. Scope
2. 开始实现
2.1. Provider
2.2. DOMCompiler
2.3. Scope
2.4. Directive
3. 一个完整的例子
4. 结论
原文：http://blog.mgechev.com/2015/03/09/build-learn-your-own-light-lightweight-angularjs/

第一次翻译外文，就拿这篇作为第一次练习。加上一些自己的理解并且做了些删减。

正文开始：

我的实践经验证明有两种好方法来学习一项新技术。

自己重新实现这个项目
分析那些你所知道的技术概念是如何运用在这个项目里的
在一些情况下第一种方式很难做到。比如，如果你为了理解kernel（linux内核）的工作原理而去重新实现一次它会很困难很慢。往往更有效的是你去实现一个轻量的版本，去除掉那些你没兴趣的技术细节，只关注核心功能。

第二种方法一般是很有效的，特别是当你具有一些相似的技术经验的时候。最好的证明就是我写的angularjs-in-patterns,对于有经验的工程师来说这是个对angular框架非常好的介绍。

不管怎么说，从头开始实现一些东西并且去理解代码使用的技术细节是非常好的学习方式。整个angularjs框架大概有20k行代码，其中有很多特别难懂的地方。这是很多聪明的程序员夜以继日的工作做出来的伟大的壮举。然而为了理解这个框架还有它主要的设计原则，我们可以仅仅简单的实现一个‘模型’。

我们可以通过下面这些步骤来实现这个模型：

简化api
去除掉对于理解核心功能无关的组件代码
这就是我在Lightweight AngularJS里面做的事情。

在开始阅读下面的内容之前，建议先了解下angularjs的基本用法，可以看这篇文章

下面是一些demo例子还有代码片段：

Lightweight AngularJS source code
Very simple todo application built with Lightweight AngularJS
让我们开始我们的实现：

主要的组件：
我们不完全实现angularjs的那套技术，我们就仅仅定义一部分的组件并且实现大部分的angularjs里面的时尚特性。可能会接口变得简单点，或者减少些功能特性。

我们会实现的angular的组件包括：

Controllers
Directives
Services
为了达到这些功能我们需要实现$compileservice(我们称之为DOMCompiler),还有$provider跟$injector(在我们的实现里统称为Provider)。为了实现双向绑定我们还要实现scope。

下面是Provider, Scope 跟 DOMCompiler 的依赖关系：

yilai

Provider
就像上面提到的，我们的Provider会包括原生angular里面的两个组件的内容：

$provide
$injector
他是一个具有如下功能特性的单列：

注册组件（directives, services 和 controllers）
解决各个组件之间的依赖关系
初始化所有组件
DOMCompiler
DOMCompiler也是一个单列，他会遍历dom树去查找对应的directives节点。我们这里仅仅支持那种用在dom元素属性上的directive。当DOMCompiler发现directive的时候会给他提供scope的功能特性（因为对应的directive可能需要一个新的scope）并且调用关联在它上面对应的逻辑代码（也就是link函数里面的逻辑）。所以这个组件的主要职责就是：

编译dom

遍历dom树的所有节点
找到注册的属性类型的directives指令
调用对应的directive对应的link逻辑
管理scope
Scope
我们的轻量级angular的最后一个主要的组件就是scope。为了实现双向绑定的功能，我们需要有一个$scope对象来挂载属性。我们可以把这些属性组合成表达式并且监控它们。当我们发现监控的某个表达式的值改变了，我们就调用对应的回调函数。

scope的职责：

监控表达式
在每次$digest循环的时候执行所有的表达式，直到稳定（译者注：稳定就是说，表达式的值不再改变的时候）
在表达式的值发生改变时，调用对应的所有的回调函数
下面本来还有些图论的讲解，但是认为意义不大，这边就略去了。

开始实现
让我们开始实现我们的轻量版angular

Provider
正如我们上面说的，Provide会：

注册组件（directives, services 和 controllers）
解决各个组件之间的依赖关系
初始化所有组件
所以它具有下面这些接口：

get(name, locals) - 通过名称 还有本地依赖 返回对应的service
invoke(fn, locals) - 通过service对应的工厂函数还有本地依赖初始化service
directive(name, fn) - 通过名称还有工厂函数注册一个directive
controller(name, fn) - 通过名称还有工厂函数注册一个controller。注意angularjs的代码里并没有controllers对应的代码，他们是通过$controller来实现的。
service(name, fn) - 通过名称还有工厂函数注册一个service
annotate(fn) - 返回一个数组，数组里是当前service依赖的模块的名称
组件的注册：

1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
var Provider = {
  _providers: {},
  directive: function (name, fn) {
    this._register(name + Provider.DIRECTIVES_SUFFIX, fn);
  },
  controller: function (name, fn) {
    this._register(name + Provider.CONTROLLERS_SUFFIX, function () {
      return fn;
    });
  },
  service: function (name, fn) {
    this._register(name, fn);
  },
  _register: function (name, factory) {
    this._providers[name] = factory;
  }
  //...
};
Provider.DIRECTIVES_SUFFIX = 'Directive';
Provider.CONTROLLERS_SUFFIX = 'Controller';
译者注：看到这里容易对controller的包装一层有疑问，先忽略，看完invoke的实现后，下面我再给出解释。

上面的代码提供了一个针对注册组件的简单的实现。我们定义了一个私有属性_provides用来存储所有的组件的工厂函数。我们还定义了directive,service和controller这些方法。这些方法本质上内部会调用_register来实现。在controller方法里面我们简单的在给的工厂函数外面包装了一层函数，因为我们希望可以多次实例化同一个controller而不去缓存返回的值。在我们看了下面的get和ngl-controller方法实现后会对controller方法有更加清晰的认识。下面还剩下的方法就是：

invoke
get
annotate
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
var Provider = {
  // ...
  get: function (name, locals) {
    if (this._cache[name]) {
      return this._cache[name];
    }
    var provider = this._providers[name];
    if (!provider || typeof provider !== 'function') {
      return null;
    }
    return (this._cache[name] = this.invoke(provider, locals));
  },
  annotate: function (fn) {
    var res = fn.toString()
        .replace(/((\/\/.*$)|(\/\*[\s\S]*?\*\/))/mg, '')
        .match(/\((.*?)\)/);
    if (res && res[1]) {
      return res[1].split(',').map(function (d) {
        return d.trim();
      });
    }
    return [];
  },
  invoke: function (fn, locals) {
    locals = locals || {};
    var deps = this.annotate(fn).map(function (s) {
      return locals[s] || this.get(s, locals);
    }, this);
    return fn.apply(null, deps);
  },
  _cache: { $rootScope: new Scope() }
};

//在get方法中我们先检测下一个组件是不是已经缓存在了私有属性_cache里面。

//如果缓存了就直接返回（译者注：这边其实就是个单列模式，只会调用注册的工厂函数一次，以后直接调用缓存的生成好的对象）。$rootScope默认就会被缓存，，因为我们需要一个单独的全局的并且唯一的超级scope。一旦整个应用启动了，他就会被实例化。

//如果不在缓存里，就从私有属性_providers里面拿到它的工厂函数，并且调用invoke去执行工厂函数实例化它。
在invoke函数里，我们做的第一件事就是判断如果没有locals对象就赋值一个空的值。   这些locals对象 叫做局部依赖
注们的实现仅仅支持解析那种作为参数属性的依赖写法：
function Controller($scope, $http) {
  // ...
}

//一旦把controller的定义转换成字符串，我们就可以很简单的通过annotate里面的正则匹配出它的依赖项。但是万一controller的定义里面有注释呢？
function Controller($scope /* only local scope, for the component */, $http) {
  // ...}
angular.controller('Controller', Controller);

//这边简单的正则就不起作用了，因为执行Controller.toString()也会返回注释，所以这就是我们为什么最开始要使用下面的正则先去掉注释：
.replace(/((\/\/.*$)|(\/\*[\s\S]*?\*\/))/mg, '').

//当我们拿到依赖项的名称后，我们需要去实例化他们。所以我们使用map来循环遍历，挨个的调用get来获取实例。

//现在我们可以这样注册组件：
Provider.service('RESTfulService', function () {
  return function (url) {
    // make restful call & return promise
  };
});

Provider.controller('MainCtrl', function (RESTfulService) {
  RESTfulService(url)
  .then(function (data) {
    alert(data);
  });
});

//然后我们可以这样执行MainCtrl：
var ctrl = Provider.get('MainCtrl' 
  + Provider.CONTROLLERS_SUFFIX);
Provider.invoke(ctrl);

DOMCompiler  (主要职责是：编译dom    )
var DOMCompiler = {
  bootstrap: function () {
    this.compile(document.children[0],
      Provider.get('$rootScope'));
  },
  compile: function (el, scope) {
    //获取某个元素上的所有指令
    var dirs = this._getElDirectives(el);
    var dir;
    var scopeCreated;
    dirs.forEach(function (d) {
      dir = Provider.get(d.name + Provider.DIRECTIVES_SUFFIX);
      //dir.scope代表当前 directive是否需要生成新的scope
      //这边的情况是只要有一个指令需要单独的scope，其他的directive也会变成具有新的scope对象，这边是不是不太好
      if (dir.scope && !scopeCreated) {
        scope = scope.$new();
        scopeCreated = true;
      }
      dir.link(el, scope, d.value);
    });
    Array.prototype.slice.call(el.children).forEach(function (c) {
      this.compile(c, scope);
    }, this);
  },
  // ...
};

//bootstrap的实现很简单。就是调用了一下compile，传递的是html的根节点，以及全局的$rootScope。在compile里面的代码就很有趣了，最开始我们使用了一辅助方法来获取某个节点上面的所有指令。

//_getElDirectives：
// ...
_getElDirectives: function (el) {
  var attrs = el.attributes;
  var result = [];
  for (var i = 0; i < attrs.length; i += 1) {
    if (Provider.get(attrs[i].name + Provider.DIRECTIVES_SUFFIX)) {
      result.push({
        name: attrs[i].name,
        value: attrs[i].value
      });
    }
  }
  return result;
}
// ...
//主要就是遍历当前节点el的所有属性，发现一个注册过的指令就把它的名字和值加入到返回的数组里。

//scope的实现：
function Scope(parent, id) {
  this.$$watchers = [];
  this.$$children = [];
  this.$parent = parent;
  this.$id = id || 0;
}
Scope.counter = 0;

//实现$watch方法：
Scope.prototype.$watch = function (exp, fn) {
  this.$$watchers.push({
    exp: exp,
    fn: fn,
    last: Utils.clone(this.$eval(exp))
  });
};


//如何新建和销毁scope
Scope.prototype.$new = function () {
  Scope.counter += 1;
  var obj = new Scope(this, Scope.counter);
  //设置原型链，把当前的scope对象作为新scope的原型，这样新的scope对象可以访问到父scope的属性方法
  Object.setPrototypeOf(obj, this);
  this.$$children.push(obj);
  return obj;
};

Scope.prototype.$destroy = function () {
  var pc = this.$parent.$$children;
  pc.splice(pc.indexOf(this), 1);
};


//脏检测$digest的实现：
Scope.prototype.$digest = function () {
  var dirty, watcher, current, i;
  do {
    dirty = false;
    for (i = 0; i < this.$$watchers.length; i += 1) {
      watcher = this.$$watchers[i];
      current = this.$eval(watcher.exp);
      if (!Utils.equals(watcher.last, current)) {
        watcher.last = Utils.clone(current);
        dirty = true;
        watcher.fn(current);
      }
    }
  } while (dirty);
  for (i = 0; i < this.$$children.length; i += 1) {
    this.$$children[i].$digest();
  }
};


//$eval（最好不要在生产环境里使用这个，这个是一个hack手段用来避免我们还需要自己做个表达式解析引擎。）

Scope.prototype.$eval = function (exp) {
  var val;
  if (typeof exp === 'function') {
    val = exp.call(this);
  } else {
    try {
      with (this) {
        val = eval(exp);
      }
    } catch (e) {
      val = undefined;
    }
  }
  return val;
};

//为了让它跑起来我们需要添加一些指令（directive）还有服务（service）。让我们来实现ngl-bind (ng-bind ), ngl-model (ng-model), ngl-controller (ng-controller) and ngl-click (ng-click)。括号里代表在angularjs里面的对应directive

//ngl-bind
Provider.directive('ngl-bind', function () {
  return {
    scope: false,
    link: function (el, scope, exp) {
      el.innerHTML = scope.$eval(exp);
      scope.$watch(exp, function (val) {
        el.innerHTML = val;
      });
    }
  };
});

//ngl-model
Provider.directive('ngl-model', function () {
  return {
    link:  function (el, scope, exp) {
      el.onkeyup = function () {
        scope[exp] = el.value;
        scope.$digest();
      };
      scope.$watch(exp, function (val) {
        el.value = val;
      });
    }
  };
});

//ngl-controller
Provider.directive('ngl-controller', function () {
  return {
    scope: true,
    link: function (el, scope, exp) {
      var ctrl = Provider.get(exp + Provider.CONTROLLERS_SUFFIX);
      Provider.invoke(ctrl, { $scope: scope });
    }
  };
});

//ngl-click
Provider.directive('ngl-click', function () {
  return {
    scope: false,
    link: function (el, scope, exp) {
      el.onclick = function () {
        scope.$eval(exp);
        scope.$digest();
      };
    }
  };
});

//测试
<!DOCTYPE html>
<html lang="en">
<head>
</head>
<body ngl-controller="MainCtrl">
  <span ngl-bind="bar"></span>
  <button ngl-click="foo()">Increment</button>
</body>
</html>

Provider.controller('MainCtrl', function ($scope) {
  $scope.bar = 0;
  $scope.foo = function () {
    $scope.bar += 1;
  };
});

//Event Loop:    主线程从"任务队列"中读取事件，这个过程是循环不断的，所以整个的这种运行机制又称为Event Loop（事件循环）。

//执行栈中的代码（同步任务），总是在读取"任务队列"（异步任务）之前执行。请看下面这个例子。

    var req = new XMLHttpRequest();
    req.open('GET', url);    
    req.onload = function (){};    
    req.onerror = function (){};    
    req.send();
//上面代码中的req.send方法是Ajax操作向服务器发送数据，它是一个异步任务，意味着只有当前脚本的所有代码执行完，系统才会去读取"任务队列"。所以，它与下面的写法等价。

    var req = new XMLHttpRequest();
    req.open('GET', url);
    req.send();
    req.onload = function (){};    
    req.onerror = function (){};
//也就是说，指定回调函数的部分（onload和onerror），在send()方法的前面或后面无关紧要，因为它们属于执行栈的一部分，系统总是执行完它们，才会去读取"任务队列"。

//Node.js也是单线程的Event Loop，但是它的运行机制不同于浏览器环境。Node.js还提供了另外两个与"任务队列"有关的方法：process.nextTick和setImmediate。

//process.nextTick方法可以在当前"执行栈"的尾部----下一次Event Loop（主线程读取"任务队列"）之前----触发回调函数。也就是说，它指定的任务总是发生在所有异步任务之前。setImmediate方法则是在当前"任务队列"的尾部添加事件，也就是说，它指定的任务总是在下一次Event Loop时执行，这与setTimeout(fn, 0)很像  so各对比下

process.nextTick(function A(){ 
 console.log(1);
 process.nextTick(function B(){
console.log(2);}); }); 
 
setTimeout(function timeout() { console.log('TIMEOUT FIRED'); }, 0) 
// 1 // 2 // TIMEOUT FIRED

setImmediate(function A() {
 console.log(1);
 setImmediate(function B(){
 console.log(2);
 }); });

 setTimeout(function timeout() { console.log('TIMEOUT FIRED'); }, 0);


//官方：'setImmediate指定的回调函数，总是排在setTimeout前面。'实际上，这种情况只发生在递归调用的时候。


setImmediate(function (){
  setImmediate(function A() {
    console.log(1);
    setImmediate(function B(){console.log(2);});
  });

  setTimeout(function timeout() {
    console.log('TIMEOUT FIRED');
  }, 0);
});
// 1
// TIMEOUT FIRED
// 2

//process.nextTick和setImmediate的一个重要区别：多个process.nextTick语句总是在当前"执行栈"一次执行完，多个setImmediate可能则需要多次loop才能执行完。事实上，这正是Node.js 10.0版添加setImmediate方法的原因，递归调用process.nextTick，将会没完没了，主线程根本不会去读取"事件队列"！




//Promise： 将返回值转换为带then方法的对象。

//常见的回调 doMission1(doMission2);  Promise模式 控制权要在我这里。你应该改变一下，你先返回一个特别的东西(promise)给我，然后我来用这个东西安排下一件事。这会变成这样doMission1().then(doMission2);

//form
function doMission1(callback){ var value = 1; callback(value); }

//to
function doMission1(){ return { then: function(callback){ var value = 1; callback(value); } }; }




//Node.js  http代理实现

var http = require('http');
var net = require('net');
var url = require('url');
function request(cReq, cRes) {
    var u = url.parse(cReq.url);
    var options = {
        hostname : u.hostname, 
        port     : u.port || 80, 
        path     : u.path,  
        method     : cReq.method,
        headers     : cReq.headers    
        };
        var pReq = http.request(
        options, function(pRes){
                              cRes.writeHead(pRes.statusCode, pRes.headers);   
                          pRes.pipe(cRes);    }).on('error',  
        function(e){
                cRes.end();    
             }); 

    cReq.pipe(pReq);
}

http.createServer().on('request', request).listen(8888, '0.0.0.0');

//运行后，会在本地 8888 端口开启 HTTP 代理服务，这个服务从请求报文中解析出请求 URL 和其他必要参数，新建到服务端的请求，并把代理收到的请求转发给新建的请求，最后再把服务端响应返回给浏览器。修改浏览器的 HTTP 代理为 127.0.0.1:8888 后再访问 HTTP 网站，代理可以正常工作。


//隧道代理
var http = require('http');
var net = require('net');
var url = require('url');
function connect(cReq, cSock) {
     var u = url.parse('http://' + cReq.url);
     var pSock = net.connect(
  u.port, u.hostname, function() {
          cSock.write(
              'HTTP/1.1 200 Connection Established\r\n\r\n'
   );
pSock.pipe(cSock);    }).on(

    'error', function(e) {
        cSock.end();
});
cSock.pipe(pSock);
}
http.createServer().on('connect', connect).listen(8888, '0.0.0.0');


//将两种代理的实现代码合二为一，就可以得到全功能的 Proxy 程序了
var http = require('http');
var net = require('net');
var url = require('url');
 
function request(cReq, cRes) {
    var u = url.parse(cReq.url);
 
    var options = {
        hostname : u.hostname, 
        port     : u.port || 80,
        path     : u.path,       
        method     : cReq.method,
        headers     : cReq.headers
    };
 
    var pReq = http.request(options, function(pRes) {
        cRes.writeHead(pRes.statusCode, pRes.headers);
        pRes.pipe(cRes);
    }).on('error', function(e) {
        cRes.end();
    });
 
    cReq.pipe(pReq);
}
 
function connect(cReq, cSock) {
    var u = url.parse('http://' + cReq.url);
 
    var pSock = net.connect(u.port, u.hostname, function() {
        cSock.write('HTTP/1.1 200 Connection Established\r\n\r\n');
        pSock.pipe(cSock);
    }).on('error', function(e) {
        cSock.end();
    });
 
    cSock.pipe(pSock);
}
 
http.createServer()
    .on('request', request)
    .on('connect', connect)
    .listen(8888, '0.0.0.0');