window.onload = function(){
	/*
	* 自由下雪 snowFall
	* author：xuanfeng
	* time: 2014-01-11
	*/
	// 控制下雪
	function snowFall(snow) {
		// 可配置属性
		snow = snow || {};
		this.maxFlake = snow.maxFlake || 200;	//最多片数
		this.flakeSize = snow.flakeSize || 10;	//雪花形状
		this.fallSpeed = snow.fallSpeed || 2;	//坠落速度
		this.status = 0;	//0-初始化、1-开始下雪、2-停止下雪、3-暂停下雪、4-继续下雪
	}

	// 兼容写法
	requestAnimationFrame = window.requestAnimationFrame ||
							window.mozRequestAnimationFrame ||
							window.webkitRequestAnimationFrame ||
							window.msRequestAnimationFrame || 
							window.oRequestAnimationFrame ||
							function(callback) { setTimeout(callback, 1000 / 60); };
	cancelAnimationFrame = window.cancelAnimationFrame ||
							window.mozCancelAnimationFrame ||
							window.webkitCancelAnimationFrame ||
							window.msCancelAnimationFrame || 
							window.oCancelAnimationFrame;

	// 开始下雪
	snowFall.prototype.start = function(){
		if(this.status == 1 || this.status == 4){
			// 已经在下雪则不作处理
			return false;
		}
		this.status = 1;

		// 创建画布
		snowCanvas.apply(this);
		// 创建雪花形状
		createFlakes.apply(this);
		// 画雪
		drawSnow.apply(this);
		if (self != top) {  
		  window.parent.start();
		}
	}

	// 停止下雪
	snowFall.prototype.stop = function(){
		if(this.status == 2 || this.status == 0 || !this.canvas){
			return false;
		}
		// 停止动画循环
		this.pause();
		this.status = 2;
		// 删除画布
		this.canvas.parentNode.removeChild(this.canvas);
		this.canvas = null;
	}

	// 暂停下雪
	snowFall.prototype.pause = function(){
		if(this.status == 3){
			return false;
		}
		this.status = 3;
		cancelAnimationFrame(this.loop)
	};
	// 继续下雪
	snowFall.prototype.resume = function(){
		if(this.status == 3 && this.canvas){
			this.status = 4;
			// 动画的计时控制
			this.loop = requestAnimationFrame(function() {
				drawSnow.apply(that)
			});
		}
	};
	

	// 创建画布
	function snowCanvas() {
		// 添加Dom结点
		var snowcanvas = document.createElement("canvas");
		snowcanvas.id = "snowfall";
		snowcanvas.width = window.innerWidth;
		snowcanvas.height = window.innerHeight;
		snowcanvas.setAttribute("style", "position: fixed; top: 0; left: 0; z-index: 2999; pointer-events: none;");
		document.getElementsByTagName("body")[0].appendChild(snowcanvas);
		this.canvas = snowcanvas;
		this.ctx = snowcanvas.getContext("2d");
		// 窗口大小改变的处理
		window.onresize = function() {
			snowcanvas.width = window.innerWidth;
			snowcanvas.height = window.innerHeight
		}
	}

	// 雪运动对象
	function flakeMove(canvasWidth, canvasHeight, flakeSize, fallSpeed) {
		this.x = Math.floor(Math.random() * canvasWidth);	//x坐标
		this.y = Math.floor(Math.random() * canvasHeight);	//y坐标
		this.size = Math.random() * flakeSize + 2;			//形状
		this.maxSize = flakeSize;							//最大形状
		this.speed = Math.random() * 1 + fallSpeed;			//坠落速度
		this.fallSpeed = fallSpeed;							//坠落速度
		this.velY = this.speed;								//Y方向速度
		this.velX = 0;										//X方向速度
		this.stepSize = Math.random() / 30;					//步长
		this.step = 0 										//步数
	}

	flakeMove.prototype.update = function() {
		var x = this.x,
			y = this.y;

		// 左右摆动(余弦)
		this.velX *= 0.98;
		if (this.velY <= this.speed) {
			this.velY = this.speed
		}
		this.velX += Math.cos(this.step += .05) * this.stepSize;

		this.y += this.velY;
		this.x += this.velX;
		// 飞出边界的处理
		if (this.x >= canvas.width || this.x <= 0 || this.y >= canvas.height || this.y <= 0) {
			this.reset(canvas.width, canvas.height)
		}
	};

	// 飞出边界-放置最顶端继续坠落
	flakeMove.prototype.reset = function(width, height) {
		this.x = Math.floor(Math.random() * width);
		this.y = 0;
		this.size = Math.random() * this.maxSize + 2;
		this.speed = Math.random() * 1 + this.fallSpeed;
		this.velY = this.speed;
		this.velX = 0;
	};

	// 渲染雪花-随机形状
	flakeMove.prototype.render = function(ctx) {
		var snowFlake = ctx.createRadialGradient(this.x, this.y, 0, this.x, this.y, this.size);
		snowFlake.addColorStop(0, "rgba(255, 255, 255, 0.9)");
		snowFlake.addColorStop(.5, "rgba(255, 255, 255, 0.5)");
		snowFlake.addColorStop(1, "rgba(255, 255, 255, 0)");
		ctx.save();
		ctx.fillStyle = snowFlake;
		ctx.beginPath();
		ctx.arc(this.x, this.y, this.size, 0, Math.PI * 2);
		ctx.fill();
		ctx.restore();
	};

	// 创建雪花-定义形状
	function createFlakes() {
		var maxFlake = this.maxFlake,
			flakes = this.flakes = [],
			canvas = this.canvas;
		for (var i = 0; i < maxFlake; i++) {
			flakes.push(new flakeMove(canvas.width, canvas.height, this.flakeSize, this.fallSpeed))
		}
	}

	// 画雪
	function drawSnow() {
		var maxFlake = this.maxFlake,
			flakes = this.flakes;
		ctx = this.ctx, canvas = this.canvas, that = this;
		// 清空雪花
		ctx.clearRect(0, 0, canvas.width, canvas.height);
		for (var e = 0; e < maxFlake; e++) {
			flakes[e].update();
			flakes[e].render(ctx);
		}
		// 一帧一帧的画
		this.loop = requestAnimationFrame(function() {
			drawSnow.apply(that);
		});
	}
	

	// 调用及控制方法
	var snow = new snowFall({maxFlake:200});
	var start = document.getElementById("start-snowFall"),
		stop = document.getElementById("stop-snowFall"),
		pause = document.getElementById("pause-snowFall"),
		resume = document.getElementById("resume-snowFall");
	// 开始
	start.onclick = function(){
		snow.start();
	}
	// 停止
	stop.onclick = function(){
		snow.stop();
	}
	// 暂停
	pause.onclick = function(){
		snow.pause();
	}
	// 恢复
	resume.onclick = function(){
		snow.resume();
	}

}