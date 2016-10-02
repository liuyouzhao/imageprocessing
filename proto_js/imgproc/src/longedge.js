var longedge = function() {}

longedge.process = function(array, width, height, t) {
	return __longedge_process(array, width, height, t);
}
/*
* Main processing function
*/
function __longedge_process(array, width, height, t) {

	var resultPixels = [];
	var arr2d = [];
	var arr2dMark = [];
	var arr2dResult = [];
	for(var i = 0; i < height; i ++) {
		arr2d[i] = [];
		arr2dMark[i] = [];
		arr2dResult[i] = [];
		for(var j = 0; j < width; j ++) {
			var index = i * width + j;
			arr2d[i][j] = array[index * 4];
			arr2dMark[i][j] = 0;
			arr2dResult[i][j] = 0;
		}
	}

	for(var i = 0; i < height; i ++) {
		for(var j = 0; j < width; j ++) {
			if(arr2d[i][j] == 0 || arr2dMark[i][j] == 1) {
				continue;
			}
		
		
			/// start...
			var queue = [];
			var saver = [];
			queue.push([j, i]);
			while(queue.length > 0) {

				var coo = queue.shift();
				var x = coo[0];
				var y = coo[1];
				/// up
				if(y - 1 >= 0 && arr2d[y - 1][x] == 255 && arr2dMark[y - 1][x] == 0) {
					queue.push([x, y - 1]);
					saver.push([x, y - 1]);
					arr2dMark[y - 1][x] = 1;
				}

				/// down
				if(y + 1 < height && arr2d[y + 1][x] == 255 && arr2dMark[y + 1][x] == 0) {
					queue.push([x, y + 1]);
					saver.push([x, y + 1]);
					arr2dMark[y + 1][x] = 1;
				}

				/// left
				if(x - 1 >= 0 && arr2d[y][x - 1] == 255 && arr2dMark[y][x - 1] == 0) {
					queue.push([x - 1, y]);
					saver.push([x - 1, y]);
					arr2dMark[y][x - 1] = 1;
				}

				/// right
				if(x + 1 < width && arr2d[y][x + 1] == 255 && arr2dMark[y][x + 1] == 0) {
					queue.push([x + 1, y]);
					saver.push([x + 1, y]);
					arr2dMark[y][x + 1] = 1;
				}

				/// up-left
				if(x - 1 >= 0 && y - 1 >= 0 && arr2d[y - 1][x - 1] == 255 && arr2dMark[y - 1][x - 1] == 0) {
					queue.push([x - 1, y - 1]);
					saver.push([x - 1, y - 1]);
					arr2dMark[y - 1][x - 1] = 1;
				}

				/// up-right
				if(x + 1 < width && y - 1 >= 0 && arr2d[y - 1][x + 1] == 255 && arr2dMark[y - 1][x + 1] == 0) {
					queue.push([x + 1, y - 1]);
					saver.push([x + 1, y - 1]);
					arr2dMark[y - 1][x + 1] = 1;
				}

				/// down-left
				if(x - 1 >= 0 && y + 1 < height && arr2d[y + 1][x - 1] == 255 && arr2dMark[y + 1][x - 1] == 0) {
					queue.push([x - 1, y + 1]);
					saver.push([x - 1, y + 1]);
					arr2dMark[y + 1][x - 1] = 1;
				}

				/// down-right
				if(x + 1 < width && y + 1 < height && arr2d[y + 1][x + 1] == 255 && arr2dMark[y + 1][x + 1] == 0) {
					queue.push([x + 1, y + 1]);
					saver.push([x + 1, y + 1]);
					arr2dMark[y + 1][x + 1] = 1;
				}
			}
			if(saver.length >= t) {
				while(saver.length > 0) {
					var coo = saver.shift();
					var x = coo[0];
					var y = coo[1];
					arr2dResult[y][x] = 255;
				}
				saver = [];
			}
		}
	}
	for(var i = 0; i < height; i ++) {
		for(var j = 0; j < width; j ++) {
			var index = i * width + j;
			resultPixels[index * 4] = arr2dResult[i][j];
			resultPixels[index * 4 + 1] = arr2dResult[i][j];
			resultPixels[index * 4 + 2] = arr2dResult[i][j];
			resultPixels[index * 4 + 3] = 255;
		}
	}
	/*=================================================================================*/
	return resultPixels;
}