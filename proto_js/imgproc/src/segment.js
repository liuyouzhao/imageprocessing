var segment = function() {}

segment.process = function(array, width, height, block, fix) {
	return __segment_process(array, width, height, block, fix);
}
/*
* Main processing function
*/

function __segment_process(array, width, height, block, fix) {
	var res = [];
	for(var i = 0; i < height / block; i ++) {
		for(var j = 0; j < width / block; j ++) {
			res = __segment_step(array, j * block, i * block, block, width, height, fix);
		}
	}
	return res;
}
function __segment_step(array, sx, sy, block, width, height, fix) {
	var vert = [];
	var hyro = [];
	var tv = 0;
	var th = 0;

	for(var i = sy, i2 = 0; i < sy + block; i ++, i2 ++) {
		var v = 0;
		for(var j = sx; j < sx + block; j ++) {
			if(array[(i * width + j) * 4] > 0) {
				v ++;
			}
		}
		hyro[i2] = v;
		tv += v;
	}
	tv = tv / block * fix;

	for(var j = sx, j2 = 0; j < sx + block; j ++, j2 ++) {
		var v = 0;
		for(var i = sy; i < sy + block; i ++) {
			if(array[(i * width + j) * 4] > 0) {
				v ++;
			}
		}
		vert[j2] = v;
		th += v;
	}
	th = th / block * fix;

	for(var i = sy, i2 = 0; i < sy + block; i ++, i2 ++) {
		for(var j = sx, j2 = 0; j < sx + block; j ++, j2 ++) {
			var x = j;
			var y = i;
			var index = y * width + x;
			var idp = index * 4;
			if(hyro[j2] < th || vert[i2] < tv) {
				array[idp] = 0;
				array[idp + 1] = 0;
				array[idp + 2] = 0;
			}
		}
	}

	/*=================================================================================*/
	return array;
}