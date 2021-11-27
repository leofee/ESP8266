<template>
	<view class="content">
		<image class="about" src="/static/about.jpg"></image>
		hello leo ,this is a page of about...
	</view>
</template>

<script>
	export default {
		data() {
			return {
				tittle:'About'
			}
		},
		onLoad() {
		
				},
		methods: {
			
		}
	}
</script>

<style>
.content {
		display: flex;
		flex-direction: column;
		align-items: center;
		justify-content: center;
	}

	.logo {
		height: 200rpx;
		width: 200rpx;
		margin-top: 200rpx;
		margin-left: auto;
		margin-right: auto;
		margin-bottom: 50rpx;
	}

	.text-area {
		display: flex;
		justify-content: center;
	}

	.title {
		font-size: 36rpx;
		color: #941d74;}
</style>
