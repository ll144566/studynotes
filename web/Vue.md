## 1、配置

### 1.1、打包问题

当前webpack版本 webpack 4.43.0

webpack ./src/main.js ./dist/bundle.js

![image-20210501142338086](..\typora-user-images\image-20210501142338086.png)



webpack ./src/main.js --output-filename ./dist/bundle.js --output-path . --mode development

或者webpack ./src/main.js -o ./dist/bundle.js --mode development

![image-20210501142510482](..\typora-user-images\image-20210501142510482.png)



### 1.2、npm init 

可用来生成package.json

~~~markdown
npm help config //打开打开帮助文档 
	例如 npm help edit、npm help config

默认值修改

npm config set init.author.email "lhongda@live.com"
npm config set init.author.name "anderson"
npm config set init.author.url "http://github.com/peacesky"
npm config set init.license "MIT"
npm config set init.version "0.1.0"
~~~



### 1.3、npm install 

npm install --save 和 npm install --save-dev

--save  <=> -S   

   --save-dev <=> -D 

npm install不会修改package.json，而后两者会将依赖添加进package.json

### 1.4、局部webpack

pack.json中的scripts的脚步在执行时，会按照一定的顺序寻找命令对应的位置。

- 首先，会寻找本地的mode_modules/.bin路径中对应的命令。
- 如果没有找到，会去全局的环境中寻找

### 1.5、loader

#### 1.5.1、样式

style-loader和ss-loader

使用webpack打包css时，出现的一个错误，导致css无法打包，原因是 css-loader 和 style-loader 版本过高。

**最新版本使用方法有变化，待后续研究**

![image-20210501162920090](..\typora-user-images\image-20210501162920090.png)

调整了下版本信息

~~~markdown
调整前
    "css-loader": "^5.2.4",
    "style-loader": "^2.0.0",
 调整后
    "css-loader": "^3.3.0",
    "style-loader": "^1.0.0",
~~~



style-loader需要放在 css-loader前

因为webpack在读使用loader的过程中，是按照从右向左的顺序读取的。

否则

![image-20210501162836358](..\typora-user-images\image-20210501162836358.png)

#### 1.5.2、图片

url-loader和file-loader

~~~javascript
    "url-loader": "^4.1.0",
    "file-loader": "^3.0.1",

publicPath: 'dist/' //涉及url会在路径前添加dist/


{
    test: /\.(png|jpg|gif)$/,
    use: [
        {
            loader: 'url-loader',
            options: {
                //加载的图片小于limit：将图片编译成base64字符串形式
                //加载的图片大于limit：使用file-loader进行加载
                limit: 8192,
                name:'img/[name].[hash:8].[ext]'//图片编译路径和命名规则
            }
        }
    ]
},
~~~

**对图片路径的处理方法常用的有两种，一种是file-loader，一种是url-loader，当我们使用其中一种是，请把另一种删掉，不然会出现图片无法正常显示的问题**

#### 1.5.3、babel-loader

把用最新标准编写的 JavaScript 代码向下编译成可以在今天随处可用的版本

~~~javascript
npm install -D babel-loader@7 babel-core babel-preset-es2015

{
    test: /\.js$/,
    exclude: /(node_modules|bower_components)/,
    use: {
        loader: 'babel-loader',
        options: {
            presets: ['es2015']
        }
    }
}
~~~

#### 1.5.4、Vue配置

**1、安装vue**

npm install vue --S

使用时可能会遇到的问题

![image-20210502112325623](..\typora-user-images\image-20210502112325623.png)

runtime-only: 代码中不可以有任何的template。

runtime-compiler 代码中可以用template，因为有compiler可以用于编译template

解决办法（两种方案）

1. 修改webpack.config.js

   ~~~javascript
   //webpack.config.js
   module.exports{
   	resolve: {
           alias:{
               'vue$':'vue/dist/vue.esm.js'
           }
       },
   } 
   ~~~

   

2. 使用render函数


   ~~~javascript
   import App from './vue/App'
   new Vue({
           el: '#app',
           render: h => h(App),
       }
   )
   ~~~

   

**2、导入Vue组件**

~~~javascript
import App from './vue/App'
~~~

![image-20210502142754297](..\typora-user-images\image-20210502142754297.png)



解决方法 npm install vue-loader vue-template-compiler --D

vue-loader加载vue文件

vue-template-compiler将.vuewen文件的template解析成render函数

需要安装vue

在 vue 工程中，安装依赖时，需要 vue 和 vue-template-compiler 版本必须保持一致，否则会报错。

使用vue-template-compiler直接读取处理了.vue文件SFC内容，将.vue文件处理为一个AST。

![image-20210502143019087](..\typora-user-images\image-20210502143019087.png)

解决方法

1. 添加插件

   ~~~javascript
   const VueLoaderPlugin = require('vue-loader/lib/plugin');
   module.exports = {
       plugins:[new VueLoaderPlugin()]
   
   }
   ~~~

2. 修改vue-loader版本，比如"vue-loader": "^13.0.0"

**3、后缀名问题**

~~~javascript
import App from './vue/App.vue'  //后缀名不可省略

//若配置了extensions 后缀名可省略
resolve: {
    extensions:['.js','.css','.vue'],
    alias:{
        'vue$':'vue/dist/vue.esm.js'
    }
},
~~~



### 1.6、plugin

#### 1.6.1、BannerPlugin

该插件是webpack内置的，主要用于对打包好的js文件的最开始处添加版权声明

~~~javascript
const webpack = require("webpack");
module.exports = {
    plugins:[
        new webpack.BannerPlugin('最终版权归qdy所有'),
    ],
}
~~~

#### 1.6.2、HtmlWebpackPlugin

安装npm install html-webpack-plugin@3.1.0 --D  **最新版本使用方法有变化，待后续研究**

publicPath: 'dist/',<font color='red'> 此时可以删掉了，因为HtmlWebpackPlugin会将index.html打包到dist目录下</font>

~~~javascript
plugins:[
	new HtmlWebpackPlugin({
    	template:'index.html' //模板
	})
]
~~~

#### 1.6.3、UglifyjsWebpackPlugin

npm install uglifyjs-webpack-plugin@1.1.1 --save-dev**最新版本使用方法有变化，待后续研究**

开发阶段不建议使用

~~~javascript
plugins:[
    new UglifyjsWebpackPlugin()
]
~~~

此时不需要BannerPlugin了，因为压缩后会被删除

### 1.7、搭建本地服务

npm install webpack-dev-server@2.9.3 --save-dev <font color = "red">最好与webpack版本对应</font>

~~~javascript
//webpack.config.js
module.exports = {
    devServer:{
        contentBase:'./dist',//为哪一个文件夹提供本地服务，默认是根文件夹
        inline:true //页面实时刷新
        //prot : 端口号
        //hsitoryApiFallback：在SPA页面中，依赖html5的history模式
    }

}

package.json
"scripts": {
   "dev": "webpack-dev-server"
},//执行 npm install dev
webpack-dev-server --open //自动打开网页    
~~~

### 1.8、配置分离

先安装 webpack-merge

npm install webpack-merge -D

![image-20210502165642249](..\typora-user-images\image-20210502165642249.png)

~~~javascript
//dev.config.js
const webpackMerge = require('webpack-merge')
const baseConfig = require('./base.config')
module.exports = webpackMerge.merge(
    baseConfig,
    {
        devServer:{
            contentBase:'./dist',
            inline:true
        }
    }
)
//prod.config.js
const  UglifyjsWebpackPlugin = require('uglifyjs-webpack-plugin')
const webpackMerge = require('webpack-merge')
const baseConfig = require('./base.config')
module.exports = webpackMerge.merge(  //"webpack-merge": "^4.1.0"版本，直接webpackMerge()
    baseConfig,
    {
        plugins:[
            new UglifyjsWebpackPlugin(),
        ]
    }
)

//base.config.js (大部分已经省略，内容主要为prod.config.js和dev.config.js共有的配置)
module.exports = {
    entry: './src/main.js',
    output: {
        path: path.resolve(__dirname,'../dist'),
        filename: 'bundle.js',
    },
}
~~~

<font color = "red">path: path.resolve(__dirname,'../dist'),：注意输出路径是相对于当前config路径</font>

~~~javascript
  "scripts": {
    "test": "echo \"Error: no test specified\" && exit 1",
    "build": "webpack --config ./build/prod.config.js",
    "dev": "webpack-dev-server --open --config ./build/dev.config.js"
  },
~~~

<font color = "red">--config 指定configure</font>



## 2、Vue-Cli2

el用于指定Vue要管理的DOM，可以帮助解析其中的指令、事件监听等等

而如果Vue实例中同时指定了template，那么template模板的内容会替换掉挂载的对应el的模板

例如：main.js页面vue对象中的template替换了index.html页面<div id=app>元素中的内容



### 2.1、脚手架的安装

```bash
1. 安装
npm install -g @vue/cli-service-global

2. 创建
vue create hello-world

3. 拉取 2.x 模板 (旧版本)
npm install -g @vue/cli-init
# `vue init` 的运行效果将会跟 `vue-cli@2.x` 相同
vue init webpack my-project
```

创建vue-cli2项目

![image-20210502174944050](..\typora-user-images\image-20210502174944050.png)

如果选择了ESLint，又不想使用了

可将以下useEslint值修改为false

~~~javascript
//config/index.js    
	// Use Eslint Loader?
    // If true, your code will be linted during bundling and
    // linting errors and warnings will be shown in the console.
    useEslint: true,
    // If true, eslint errors and warnings will also be shown in the error overlay
    // in the browser.
~~~

runtime-compiler template -> ast -> render -> vdom -> UI

runtime-only render -> vdom -> UI



~~~javascript
/*1. 普通用法*/
new Vue({
  el: '#app',
  //App会替换 'App'
  render(createElement){
    //createElement可以接收三个参数，标签名，class和内容,内容里面可以通过createElement接着创建标签。
    return createElement('h2',{class:'box'},['hello word',
      createElement('button','按钮')])
  }
})

/*2.runtime-only用法*/
new Vue({
  el: '#app',
  //App会替换 'App'
  render: h => h(App)
})
~~~

![image-20210503113438666](..\typora-user-images\image-20210503113438666.png)

### 2.2、runtime-only 和 runtime-compiler

#### 2.2.1、区别

1. runtime-only 比 runtime-compiler 轻 6kb

2. runtime-only 运行更快

3. runtime-only 其实只能识别render函数，不能识别template，.vue文件中的也是被 vue-template-compiler 翻译成了  render函数，所以只能在.vue里写 template

4. 最终渲染成真实DOM的过程：
   -  runtime-compiler：template -> ast -> render -> vdom -> UI
   - runtime-only：render -> vdom -> UI

#### 2.2.2、runtime + compiler 中 Vue 的运行过程

3、runtime + compiler 中 Vue 的运行过程:

(1)首先将vue中的模板进行解析解析成abstract syntax tree （ast）抽象语法树

(2)将抽象语法树在编译成render函数

(3)将render函数再翻译成virtual dom 虚拟dom

(4)将虚拟dom显示在浏览器上

#### 2.2.3、runtime-only 更快的原因：

runtime-only比runtime-compiler更快，因为它省略了vue内部过程中的第一个过程，如果是runtime-compiler那么main.js中就会出现template从而需要过程一导致增加了一个过程，同时增加了大小而 runtime-only 模式中不是没有写 template ，只是把 template 放在了.vue 的文件中了并有一个叫 vue-template-compiler的在开发依赖时将.vue文件中的 template 解析成 render 函数了因为是开发依赖，不在最后生产中，所以最后生产出来的运行的代码没有template

#### 2.2.4、简单总结

如果在之后的开发中，你依然使用template，就需要选择runtimecompiler
如果你之后的开发中，使用的是.vue文件夹开发，那么可以选择runtimeonly

## 3、Vue-Cli4

vue create hello-world



vue-cli3开始默认配置了git

配置进行了隐藏

路径：node_modules/@vue/cli-service/webpack.config,js



vue ui可打开配置页面

若确实需要修改，可添加vue.config.js  <font color = "red">注意配置文件名</font>

## 4、路由

location.hash

history.pushState

history.replaceState

history.go(-1)  <=> history.back()

history.go(1)   <=>  history.forward() 

### 4.1、vue-router的简单使用

~~~javascript
// /router/index.js
import VueRouter from 'vue-router'
import Vue from 'vue'
import home from '../components/Home'
import about from '../components/about'
Vue.use(VueRouter)

const routes = [
  {
    path: '',
    redirect:'/home' //重定向
  },
  {
    path:'/home',
    component:home
  },
  {
    path:'/about',
    component:about
  }
]

const vueRouter = new VueRouter({
  routes
});

export default vueRouter

//App.vue
<template>
  <div id="app">
    <router-link to="/home">首页</router-link>
    <router-link to="/about">关于</router-link>
    <router-view></router-view>
  </div>
</template>

<script>
export default {
  name: 'App'
}
</script>

<style>

</style>


//main.js
import Vue from 'vue'
import App from './App'
import router from "./router"; //会自动去找文件夹里面的index
//import vueRouter from "./router/index";
Vue.config.productionTip = false

/* eslint-disable no-new */
new Vue({
  el: '#app',
  router,
  render: h => h(App)
})


~~~

#### 4.1.1、模式

默认为hash模式

![image-20210504114650177](..\typora-user-images\image-20210504114650177.png)

可修改为history模式

![image-20210504114729568](..\typora-user-images\image-20210504114729568.png)

~~~javascript
const vueRouter = new VueRouter({
  routes,
  mode:'history'
});
~~~

### 4.2、router-link

#### 4.2.1、tag

默认为a标签

可渲染为其它属性

~~~html
<router-link to="/home" tag="button">首页</router-link>
~~~

#### 4.2.2、replace

默认使用push，可返回，前进

replace无history

~~~html
<router-link to="/home" tag="button" replace>首页</router-link>
~~~

#### 4.2.3、active-class

活跃标签会有一个默认为router-link-active类型

![image-20210504115756157](..\typora-user-images\image-20210504115756157.png)

1. 可通过active-class修改

~~~html
<template>
  <div id="app">
    <router-link to="/home"  replace active-class="active">首页</router-link>
    <router-link to="/about" replace active-class="active">关于</router-link>
    <router-view></router-view>
  </div>
</template>
~~~

2. 可通过全局修改（当前router有效）

~~~javascript
const vueRouter = new VueRouter({
  routes,
  mode:'history',
  linkActiveClass:'active'
});
~~~

####  4.2.4、其它方式实现router-link

~~~javascript
<button @click="linkToHome">首页</button>
<button @click="linkToAbout">关于</button>

//添加点击事件
export default {
  name: 'App',
  methods:{
    linkToHome(){
      this.$router.push("/home")
    },
    linkToAbout(){
      this.$router.push("/about")
    }
  }
}
//$router是值创建的vueRouter
~~~

### 4.3、动态路由

~~~html
<router-link :to="'/user/'+name">用户</router-link>
~~~



~~~javascript
  {
    path:'/user/:username',
    component:user
  }
~~~



~~~javascript
<template>
    <div>
      <h2>我是用户界面</h2>
      <p>我是用户信息</p>
      <p>{{userId}}</p>
    </div>
</template>

<script>
    export default {
        name: "User",
        computed:{
          userId(){
            //$route之当前活跃路由
            return this.$route.params.username
          }
        }
    }
</script>
~~~

### 4.3、打包文件的解析

![image-20210504144612557](..\typora-user-images\image-20210504144612557.png)

### 4.5、路由懒加载的使用

~~~javascript
// import home from '../components/Home'
// import about from '../components/about'
// import user from '../components/User'

const home = ()=> import('../components/Home')
const about = ()=> import('../components/about')
const user = ()=>import('../components/User')
~~~

一个懒加载对应一个js文件

![image-20210504152000101](..\typora-user-images\image-20210504152000101.png)



### 4.6、嵌套路由

<font color = "red">注意：</font>

关于`<router-link to="message">message</router-link>` to后面的参数

例如`http://localhost:8080/home/news`

<font color = "red">`<router-link to="message">message</router-link>`结果为</font>

`http://localhost:8080/home/message`  **替换最后一个路径**

<font color = "red">`<router-link to="/message">message</router-link>`结果为</font>

`http://localhost:8080/message`  **替换当前url的全部路径**



可在路由配置中添加子路由

~~~javascript

  {
    path:'/home',
    component:home,
    children:[
      {
        path: '',
        redirect:'/home/news',
      },
      {
        path:'/home/news',
        component:homeNews
      },
      {
        path: '/home/message',
        component: homeMessage
      }
    ]
  },
      
    //也可以使用如下写法，个人不推荐，可能会出问题  
  {
    path:'/home',
    component:home,
    children:[
      {
        path: '',
        redirect:'news',
      },
      {
        path:'news',
        component:homeNews
      },
      {
        path: 'message',
        component: homeMessage
      }
    ]
  },
~~~



~~~html
<!--home.vue-->
<template>
    <div>
      <h2>我是首页</h2>
      <p>我是首页内容</p>
      <hr>
      <router-link to="/home/news">news</router-link>
      <router-link to="/home/message">message</router-link>
      <router-view></router-view>
    </div>
</template>

<!--也可以使用如下写法，个人不推荐，可能会出问题  -->

<template>
    <div>
      <h2>我是首页</h2>
      <p>我是首页内容</p>
      <hr>
      <router-link to="news">news</router-link>
      <router-link to="message">message</router-link>
      <router-view></router-view>
    </div>
</template>
~~~



**可能出现的问题**

比如当前情况下点击profile，正常来说url应该要变成`localhost:8080/profile`

![image-20210504162050568](..\typora-user-images\image-20210504162050568.png)

如果`<router-link to="profile">message</router-link>`则点击profile后url会变成`localhost:8080/home/profile`

![image-20210504162321594](..\typora-user-images\image-20210504162321594.png)

### 4.7、vue-router参数传递

两种方式

~~~html
    <router-link :to="{
      path:'profile',
      query:{
        name:'Apricity',
        age:18
      }
    }">profile</router-link>
~~~



~~~javascript
    linkToProfile(){
      this.$router.push({
        path:"/profile",
        query:{
          name:'Apricity',
          age:18
        }
      })
    }
~~~



### 4.8、router和route的由来（vue-router 3.0.2）

~~~javascript
Vue.use(VueRouter)会调用  VueRouter的install方法

//index.js
import { install } from './install'
...
VueRouter.install = install

//install.js

export function install (Vue){
  ...
  Object.defineProperty(Vue.prototype, '$router', {
    get () { return this._routerRoot._router }
  })

  Object.defineProperty(Vue.prototype, '$route', {
    get () { return this._routerRoot._route }
  })
  ...
  Vue.component('RouterView', View)
  Vue.component('RouterLink', Link)
}
~~~



所有vue组件都继承自vue的原型



### 4.9、全局守卫

#### 4.9.1、[全局前置守卫](https://router.vuejs.org/zh/guide/advanced/navigation-guards.html#全局前置守卫)

~~~javascript

//beforeEach 全局前置首位
vueRouter.beforeEach((to,from,next)=>{
  //document.title = to.meta.title; 此方法嵌套路由时可能会出现问题
  console.log(to);
  document.title = to.matched[0].meta.title;
  next()
})


~~~

每个守卫方法接收三个参数：

- **`to: Route`**: 即将要进入的目标 [路由对象](https://router.vuejs.org/zh/api/#路由对象)
- **`from: Route`**: 当前导航正要离开的路由
- **`next: Function`**: 一定要调用该方法来 **resolve** 这个钩子。执行效果依赖 `next` 方法的调用参数。
  - **`next()`**: 进行管道中的下一个钩子。如果全部钩子执行完了，则导航的状态就是 **confirmed** (确认的)。
  - **`next(false)`**: 中断当前的导航。如果浏览器的 URL 改变了 (可能是用户手动或者浏览器后退按钮)，那么 URL 地址会重置到 `from` 路由对应的地址。
  - **`next('/')` 或者 `next({ path: '/' })`**: 跳转到一个不同的地址。当前的导航被中断，然后进行一个新的导航。你可以向 `next` 传递任意位置对象，且允许设置诸如 `replace: true`、`name: 'home'` 之类的选项以及任何用在 [`router-link` 的 `to` prop](https://router.vuejs.org/zh/api/#to) 或 [`router.push`](https://router.vuejs.org/zh/api/#router-push) 中的选项。
  - **`next(error)`**: (2.4.0+) 如果传入 `next` 的参数是一个 `Error` 实例，则导航会被终止且该错误会被传递给 [`router.onError()`](https://router.vuejs.org/zh/api/#router-onerror) 注册过的回调。

#### 4.9.2、[全局解析守卫](https://router.vuejs.org/zh/guide/advanced/navigation-guards.html#全局解析守卫)

> 2.5.0 新增

在 2.5.0+ 你可以用 `router.beforeResolve` 注册一个全局守卫。这和 `router.beforeEach` 类似，区别是在导航被确认之前，**同时在所有组件内守卫和异步路由组件被解析之后**，解析守卫就被调用。

#### 4.9.3、[全局后置钩子](https://router.vuejs.org/zh/guide/advanced/navigation-guards.html#全局后置钩子)

你也可以注册全局后置钩子，然而和守卫不同的是，这些钩子不会接受 `next` 函数也不会改变导航本身：

~~~javascript
router.afterEach((to, from) => {
  // ...
})
~~~

#### 4.9.4、路由独享的守卫

你可以在路由配置上直接定义 `beforeEnter` 守卫：

```js
const router = new VueRouter({
  routes: [
    {
      path: '/foo',
      component: Foo,
      beforeEnter: (to, from, next) => {
        // ...
      }
    }
  ]
})
```

这些守卫与全局前置守卫的方法参数是一样的。

#### 4.9.5、[组件内的守卫](https://router.vuejs.org/zh/guide/advanced/navigation-guards.html#组件内的守卫)

最后，你可以在路由组件内直接定义以下路由导航守卫：

- `beforeRouteEnter`
- `beforeRouteUpdate` (2.2 新增)
- `beforeRouteLeave`

```js
const Foo = {
  template: `...`,
  beforeRouteEnter(to, from, next) {
    // 在渲染该组件的对应路由被 confirm 前调用
    // 不！能！获取组件实例 `this`
    // 因为当守卫执行前，组件实例还没被创建
  },
  beforeRouteUpdate(to, from, next) {
    // 在当前路由改变，但是该组件被复用时调用
    // 举例来说，对于一个带有动态参数的路径 /foo/:id，在 /foo/1 和 /foo/2 之间跳转的时候，
    // 由于会渲染同样的 Foo 组件，因此组件实例会被复用。而这个钩子就会在这个情况下被调用。
    // 可以访问组件实例 `this`
  },
  beforeRouteLeave(to, from, next) {
    // 导航离开该组件的对应路由时调用
    // 可以访问组件实例 `this`
  }
}
```

`beforeRouteEnter` 守卫 **不能** 访问 `this`，因为守卫在导航确认前被调用，因此即将登场的新组件还没被创建。

不过，你可以通过传一个回调给 `next`来访问组件实例。在导航被确认的时候执行回调，并且把组件实例作为回调方法的参数。

```js
beforeRouteEnter (to, from, next) {
  next(vm => {
    // 通过 `vm` 访问组件实例
  })
}
```

注意 `beforeRouteEnter` 是支持给 `next` 传递回调的唯一守卫。对于 `beforeRouteUpdate` 和 `beforeRouteLeave` 来说，`this` 已经可用了，所以**不支持**传递回调，因为没有必要了。

```js
beforeRouteUpdate (to, from, next) {
  // just use `this`
  this.name = to.params.name
  next()
}
```

这个离开守卫通常用来禁止用户在还未保存修改前突然离开。该导航可以通过 `next(false)` 来取消。

```js
beforeRouteLeave (to, from, next) {
  const answer = window.confirm('Do you really want to leave? you have unsaved changes!')
  if (answer) {
    next()
  } else {
    next(false)
  }
}
```

#### 4.9.6、[完整的导航解析流程](https://router.vuejs.org/zh/guide/advanced/navigation-guards.html#完整的导航解析流程)

1. 导航被触发。
2. 在失活的组件里调用 `beforeRouteLeave` 守卫。
3. 调用全局的 `beforeEach` 守卫。
4. 在重用的组件里调用 `beforeRouteUpdate` 守卫 (2.2+)。
5. 在路由配置里调用 `beforeEnter`。
6. 解析异步路由组件。
7. 在被激活的组件里调用 `beforeRouteEnter`。
8. 调用全局的 `beforeResolve` 守卫 (2.5+)。
9. 导航被确认。
10. 调用全局的 `afterEach` 钩子。
11. 触发 DOM 更新。
12. 调用 `beforeRouteEnter` 守卫中传给 `next` 的回调函数，创建好的组件实例会作为回调函数的参数传入。





### 4.10、keep-alive

keep-alive是vue内置的一个组件

~~~html
<keep-alive>
    <router-view/>
</keep-alive>
~~~

<font color = "red">activated
deactivated在设置了 keep-alive之后才会被执行</font>

保存嵌套路由的状态

~~~javascript
export default {
  data(){
    return{
      path:"/home/news",
    }
  },

  activated() {
    this.$router.push(this.path)
    console.log("activated")
  },
  // deactivated() { //此时path = 下一个路由的路径
  //   this.path=this.$route.path
  //   console.log("deactivated");
  // }

  beforeRouteLeave(to,from,next){
    this.path=this.$route.path
    next()
  }
}
~~~

#### 4.10.1、exclude和include

```html
<keep-alive exclude="Profile">
  <router-view/>
</keep-alive>

<keep-alive include="Profile">
  <router-view/>
</keep-alive>
这里的name为Vue里面的name
```



## webpack.config.js

路径别名

~~~javascript
//路径更改别名需要重新 npm run dev
resolve: {
  extensions: ['.js', '.vue', '.json'],
  alias: {
    '@': resolve('src'),
    'assets':resolve('src/assets'),
    'components':resolve('src/components'),
    'view':resolve('src/view'),
  }
},
import TabBar from "components/tabbar/TabBar";
import TabBarItem from "components/tabbar/TabBarItem";
~~~

~~~html
需要加~
<TabBarItem path="/home" activeColor="green">
  <img src="~assets/img/tabbar/home-deactivated.svg" alt="" slot="item-icon">
  <img src="~/assets/img/tabbar/home-activated.svg" alt="" slot="item-icon-active">
  <div slot="item-text">首页</div>

</TabBarItem>
~~~

##  5、VueX

~~~javascript
import vue from 'vue'
import vuex from 'vuex'
vue.use(vuex)

export default new vuex.Store({
  state: {
    
  }
  ,
  mutations:{

  },
  actions:{

  },
  getters:{

  },
  modules:{

  }
})
~~~



![vuex](https://vuex.vuejs.org/vuex.png)

### 5.1、state

相当于vue里面的data

### 5.2、getters

相当于计算属性

~~~javascript
export default new vuex.Store({
  state: {
    count: 0,
    students: [
      {id: 110, name: 'Tom', age: 18},
      {id: 111, name: 'Jerry', age: 19},
      {id: 112, name: 'Michael', age: 20},
      {id: 113, name: 'tom', age: 21}
    ]
  } ,
  getters:{
    powerCount(state){
      return state.count*state.count
    },
    moreThan20Years(state){
      return state.students.filter(s => s.age > 20)
    },

      //可以传递两个参数
    moreThan20YearsSum(state,getter){
      return getter.moreThan20Years.length
    },
    moreThanXYears(state){
      // return function (x) {
      //   return state.students.filter(s => s.age > x)
      // }

      return x=>state.students.filter(s => s.age > x)

    },

  },
  modules:{

  }
})
~~~

<font color = "red">如果使用计算属性时需要参入参数，可使用闭包</font>

### 5.3、mutations

 携带参数

#### 5.3.1、普通的提交封装

~~~javascript
//函数
mutations:{
    incrementNum(state,num){
    	state.count = state.count+num;
	}
}


//调用 多个参数可以封装成对象，参数成为mutation载荷(payload)
additionWidthNum(){
    this.$store.commit("incrementNum",5)
}
~~~

#### 5.3.2、特殊的提交封装

~~~javascript
mutations{
    incrementStudent(state,payload){
    	state.students.push(payload.student)
	}
}

additionStudent(){
    this.$store.commit({
        type:"incrementStudent",
        student:{id: 114, name: 'Maria', age: 22},
    })
}
~~~

<font color = "red">注意此时的参数payload是一个对象，student是其一个属性</font>

![image-20210505224458950](..\typora-user-images\image-20210505224458950.png)

**vuex数据的响应原理**

state中的这些属性都会被加入到响应式系统中，而响应式系统会监听属性的变化，当属性发生变化时，会通知所有界面中用到该属性的地方，让界面发生刷新。

如果是新加或删除的属性，则需通过Vue.set(obj,key,value)或Vue.delete

#### 5.3.2、常量类型函数名

建议函数名使用常量类型，

~~~javascript
export const INCREMENT = 'increment'

import {INCREMENT} from './mutations.type' 

mutations:{
	increment(state){
      state.count++
    },

    ['increment'](state){
      state.count++
    },
        
    [INCREMENT](state){
    	state.count++
	},
}    
        


        
addition(){
    this.$store.commit(INCREMENT)
},
~~~

### 5.4、actions

mutations中不要执行异步操作

~~~javascript
updateInfo(){
  this.$store.dispatch("aUpdateInfo") 
  
  this.$store.dispatch({ //payload 与mutations中的payload用法一致
    type:"aUpdateInfo",
    callback(){
      console.log("完成更新");
    }
  })
}

actions:{
  aUpdateInfo(context){
    setTimeout(()=>{
      context.commit("updateInfo") //注意数据修改一定要通过mutations
    },1000)
  }

},
    
mutations:{
  updateInfo(state){
    //1.不推荐耗时操作
    // setTimeout(()=>{
    //   state.info.name="666"
    // },1000)
    state.info.name="666"
  }
},
~~~

**使用promise添加回调函数**

~~~javascript
updateInfo(){
    this.$store.dispatch("aUpdateInfo")
  .then(info => console.log(info))
}

actions:{
  aUpdateInfo(context){
      return new Promise((resolve => {
          setTimeout(()=>{
              context.commit("updateInfo")
              resolve("完成更新！！！")
          },1000)
      }))
  }

},
    
mutations:{
  updateInfo(state){
    state.info.name="666"
  }
},
~~~



### 5.5、module

由于使用单一状态树，应用的所有状态会集中到一个比较大的对象。当应用变得非常复杂时，store 对象就有可能变得相当臃肿。

为了解决以上问题，Vuex 允许我们将 store 分割成**模块（module）**。每个模块拥有自己的 state、mutation、action、getter、甚至是嵌套子模块——从上至下进行同样方式的分割：

```js
const moduleA = {
  state: () => ({ ... }),
  mutations: { ... },
  actions: { ... },
  getters: { ... }
}

const moduleB = {
  state: () => ({ ... }),
  mutations: { ... },
  actions: { ... }
}

const store = new Vuex.Store({
  modules: {
    a: moduleA,
    b: moduleB
  }
})

store.state.a // -> moduleA 的状态
store.state.b // -> moduleB 的状态
```



默认情况下，模块内部的 action、mutation 和 getter 是注册在**全局命名空间**的——这样使得多个模块能够对同一 mutation 或 action 作出响应。

子module中的action、mutation 和 getter 使用方法与跟module相同



**state使用方法略有区别**

~~~javascript
const moduleA = {
  state: {
    name:"我是moduleA"
  }
  ,
  mutations:{

  },
  actions:{

  },
  getters:{

  },
  modules:{

  }
}

modules:{
    moduleA
}


<p>{{$store.state.moduleA.name}}</p>
~~~

![image-20210509185857313](..\typora-user-images\image-20210509185857313.png)





## 6、axios

### 6.1、[基本的示例](https://cn.vuejs.org/v2/cookbook/using-axios-to-consume-apis.html#基本的示例)

有很多时候你在构建应用时需要访问一个 API 并展示其数据。做这件事的方法有好几种，而使用基于 promise 的 HTTP 客户端 [axios](https://github.com/axios/axios) 则是其中非常流行的一种。

我们有很多种方式可以从 API 请求信息，但是最好首先确认这些数据看起来长什么样，以便进一步确定如何展示它。为此，我们会调用一次这个 API 并输出结果，以便我们能够看清楚它。如 CoinDesk 的 API 文档所述，请求会发送到 `https://api.coindesk.com/v1/bpi/currentprice.json`。所以，我们首先创建一个 data 里的 property 以最终放置信息，然后将会在 `mounted` 生命周期钩子中获取数据并赋值过去：

```
new Vue({
  el: '#app',
  data () {
    return {
      info: null
    }
  },
  mounted () {
    axios
      .get('https://api.coindesk.com/v1/bpi/currentprice.json')
      .then(response => (this.info = response))
  }
})
```



~~~javascript
axios({
  url:"http://123.207.32.32:8000/home/multidata"
}).then(res=>{
  console.log(res);
})
~~~



等待两个请求都完成，res为数组

~~~javascript
axios.all([axios({
  url:"http://123.207.32.32:8000/home/multidata"
}),axios({
  url:"https://api.coindesk.com/v1/bpi/currentprice.json"
})]).then(res=>console.log(res))
~~~

也可以使用axios.spread分别获取返回值

~~~javascript
axios.all([axios({
  url:"http://123.207.32.32:8000/home/multidata"
}),axios({
  url:"https://api.coindesk.com/v1/bpi/currentprice.json"
})]).then(axios.spread((res1,res2)=>{
  console.log(res1);
  console.log(res2);
}))
~~~

### 6.2、配置信息

```
axios.defaults.baseURL = "http://123.207.32.32:8000"
axios.defaults.timeout = 500
```



### 6.3、axios的实例与封装

每个实例都有独立的配置

~~~javascript
const instance_01 = axios.create({
  baseURL:"http://123.207.32.32:8000",
  timeout:500
})

instance_01({
  url:"home/multidata"
}).then(res=>{
  console.log(res);
})
~~~



封装axios，方便维护

~~~javascript
import axios from "axios";

const instance_base = axios.create({
  baseURL:"http://123.207.32.32:8000",
  timeout:500
})


export function instance_02(config){
  return new Promise((resolve, reject) => {
    instance_base({
      url:"home/multidata"
    }).then(data=>{
      resolve(data)
    })
  })
}


export function instance_03(config){
  return instance_base({
    url:"/home/multidata"
  })
}
~~~

### 6.4、axios拦截器

~~~javascript
//请求拦截
instance_base.interceptors.request.use(config=>{
  console.log(config)
  return config //一定要返回
},error => {
  console.log(error)
})

//响应拦截
instance_base.interceptors.response.use(res=>{
  console.log(res)
  return res.data //可以只返回data(需要的数据)
},error => {
  console.log(error)
})
~~~

