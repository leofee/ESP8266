<template>
	<view class="root-wrapper">
		<view class="root">
			<view class="header-wrapper">												
				<view class="header-content-wrapper">
					<view class="header-title">
						<text>{{airText}}</text>
						<text>{{area}}-{{city}}</text>
					</view>
					<view class="header-text">
						<text>{{airValue}}°C</text>
						<text>{{weather}}</text>
					</view>
					<view class="weather-advice">
						{{weatherAdvice}}
					</view>
				</view>
			</view>
			<view class="body-wrapper">
				<view class="body">
					<view class="data-wrapper">
						<view class="data">							
								<view class="data-left">
									<image class="data-icon" src="../../static/icons/Led.png" ></image>
									<view class="data-title">Led灯</view>
								</view>
								<view class="data-value">
									<switch @change="onLedChange" :checked="Led" color="#55aa7f"></switch>
								</view>							
						</view>
						
						<view class="data">
								<view class="data-left">
									<image class="data-icon" src="../../static/icons/Alert.png" ></image>
									<view class="data-title">报警器</view>
								</view>
								<view class="data-value">
									<switch @change="onBeepChange" :checked="Beep" color="#ffca43"></switch>
								</view>							
						</view>
					</view>
					
					<view class="data-wrapper">
						<view class="data">							
								<view class="data-left">
									<image class="data-icon" src="../../static/icons/router.png" ></image>
									<view class="data-title">家庭网关</view>
								</view>
								<view class="data-value">
									在线
								</view>							
						</view>
						
						<view class="data">
								<view class="data-left">
									<image class="data-icon" src="../../static/icons/temp.png" ></image>
									<view class="data-title">室内温度</view>
								</view>
								<view class="data-value">
									{{Temp}}°C
								</view>							
						</view>
					</view>
					
					<view class="data-wrapper">
						<view class="data">							
								<view class="data-left">
									<image class="data-icon" src="../../static/icons/Hum.png" ></image>
									<view class="data-title">室内湿度</view>
								</view>
								<view class="data-value">
									{{Hum}}%
								</view>							
						</view>
						
						<view class="data">
								<view class="data-left">
									<image class="data-icon" src="../../static/icons/Motor.png" ></image>
									<view class="data-title">电机开关</view>
								</view>
								<view class="data-value">
									待开发
								</view>							
						</view>
					</view>
					
				</view>
			</view>
			
		</view>
	</view>
</template>

<script>
	
	import { connect } from "mqtt"
	const mqttUrl = 'wss://mqtt.luvart.xyz:8084/mqtt'
	export default {
			
		data() {
			return {	
				client:{},
				Temp:0,
				Hum:0,
				Led:false,
				Beep:false,
				area:'***',//市区
				city:'***',//城市
				airText:'***',//空气优良
				airValue:'0',//空气指数
				weather:'***',//天气
				weatherAdvice:'***********',//天气建议
							
			}
		},
		
		
		onLoad() {
			let that = this
			uni.getLocation({
			    type: 'wgs84',
			    success: function (res) {
			       const latitude = res.latitude;
				   const longitude = res.longitude;
				   const key = "0ee6d84ceef0445f9c02912dbb5664df";
				   uni.request({
				       url: `https://devapi.qweather.com/v7/weather/now?location=${longitude},${latitude}&key=${key}`, 
				       success: (res) => {
				           //console.log(res.data);
				          const now = res.data.now;
						  console.log(now)						  
						  that.weather = now.text
						  that.airValue = now.temp
						  that.airText = now.windDir
				       }
				   });
				   
				   uni.request({
				       url: `https://devapi.qweather.com/v7/indices/1d?type=1,2&location=${longitude},${latitude}&key=${key}`, 
				       success: (res) => {
				           //console.log(res.data);
				          const daily = res.data.daily;
				   		  console.log(daily)						  				   		 
				   		  that.weatherAdvice = daily[0].text
				       }
				   });
				   
				   uni.request({
				       url: `https://geoapi.qweather.com/v2/city/lookup?location=${longitude},${latitude}&key=${key}`, 
				       success: (res) => {
				          console.log(res.data);
				          const Location = res.data.location;
				   		  console.log(Location)	
						  that.city = Location[0].adm1
						  that.area = Location[0].name
				       }
				   });
				  
			    }
			});
			
		},
		
		
		
		
		methods: {
         		onLedChange(event){
				 var that = this
				 console.log(event.detail)
				 let sw = event.detail.value
				 if(sw){
					 that.client.publish('/NodeMcu/sub','{"led":1}',function(err){
						 if(!err){
							 console.log("成功下发命令--开灯")
						 }
					 })
				 }else{
					 that.client.publish('/NodeMcu/sub','{"led":0}',function(err){
					 	if(!err){
					 		 console.log("成功下发命令--关灯")
					 		
						}
					 })
				 }
				},
				
				onBeepChange(event){
				 var that = this
				 console.log(event.detail)
				 let sw = event.detail.value
				 if(sw){
					 that.client.publish('/NodeMcu/sub','{"beep":1}',function(err){
						 if(!err){
							 console.log("成功下发命令--打开报警")
						 }
					 })
				 }else{
					 that.client.publish('/NodeMcu/sub','{"beep":0}',function(err){
					 	if(!err){
					 		 console.log("成功下发命令--关闭报警")
					 		
						}
					 })
				 }
				},
				
				
		},
		onShow() {
			var that = this		
			that.client = connect(mqttUrl)
			that.client.on('connect',function(){
				console.log("成功连接mqtt服务器")
			    that.client.subscribe("/NodeMcu/pub",function(err){
					if(!err){
						console.log("成功订阅")
					}
				})
														   
			})
			
			that.client.on('message',function(topic,message){
				console.log(topic)				
				//message是16进制的buffer字节流
				let dataFromDev = {}
				dataFromDev = JSON.parse(message)
				console.log(dataFromDev)
				that.Temp = dataFromDev.Temp
				that.Hum = dataFromDev.Hum
				that.Led = dataFromDev.Led
				that.Beep = dataFromDev.Beep
			})
		}										 
				    
				
		
	}
</script>

<style>
	.root-wrapper{
		min-height: 100vh;
		overflow:hidden;
	}
	.header-wrapper{
		background: rgba(0,0,0,0.5) no-repeat center center;
		background-blend-mode: multiply;
		height:34vh;
		color:#fff;
		background-image: url(../../static/venus.jpg);
		background-size:cover; 
		background-repeat: no-repeat;		
	}
	.header-content-wrapper{
		padding: 100px 20px 20px 20px;
	}
	.header-title{
		display:flex;
		justify-content: space-between;
	}
	.header-text{
		font-size: 32px;
		font-weight: 400;
		display: flex;
		justify-content: space-between;
		
	}
	.weather-advice{
		margin-top: 20px;
		font-size: 12px;
	}
	.body-wrapper{
		padding: 20px 15ps 0 15px;
		
	}
	.data-wrapper{
		margin-top: 15px;
		display: flex;
		justify-content: space-around;
	}
	.data{
		background-color: #2f2f2f;
		width:150px;
		height:80px;
		display: flex;
		justify-content: space-between;
		border-radius: 5px;
		transition: transform 0.1s ease-in-out;
	}
	.data:active{
		transform: scale(0.95);
	}
	.data-left{
		margin-top: 12px;
		margin-left: 16px;
	}
	.data-icon{
		height:36px;
		width:36px;
	}
	.data-title{
		font-size: 12px;
		color:#7f7f7f;
	}
	.data-value{
		color:#7f7f7f;
		font-size: 20px;
		margin-top: 10px;
		margin-right: 16rpx;
	}
</style>
