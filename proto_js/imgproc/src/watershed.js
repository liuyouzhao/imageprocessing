var watershed = function() {}

watershed.process = function(pixels, width, height, coreW, threadhold) {
	return __watershed_process(pixels, width, height, coreW, threadhold);
}
/*
* Main processing function
*/
function __watershed_process(pixels, width, height, coreW, threadhold) {

	var resultPixels = [];

	var mapW = Math.floor(width / coreW);
	var mapH = Math.floor(height / coreW);

	var mapResult = [];
	var mw = Math.floor(width / coreW);
	var mh = Math.floor(height / coreW);

	clearPixels(mapResult, mw, mh, 1);

    /*===============================================================================*/
	/* Our teacher said that all things are some "for" loop, here it is
	 * Change the code below, with your knowledge from books.
	*/
	for(var i = 0, j = 0; i < height * width * 4; i += 4, j ++) {

		var r = pixels[i];
		var g = pixels[i + 1];
		var b = pixels[i + 2];

		var grey = (r + g + b) / 3;

		var x = j % width;
		var y = Math.floor(j / width);

		var mx = Math.floor(x / coreW);
		var my = Math.floor(y / coreW);

		mapResult[my * mw + mx] += grey;
	}

	for(var i = 0; i < mw * mh; i ++) {
		mapResult[i] = Math.floor(mapResult[i] / (coreW * coreW));
		if(mapResult[i] >= threadhold) {
			mapResult[i] = 255;
		}
		else {
			mapResult[i] = 0;
		}
		var iy = Math.floor(i / mw);
		var ix = i % mw;
		for(var y = iy * coreW; y < iy * coreW + coreW; y ++) {
			for(var x = ix * coreW; x < ix * coreW + coreW; x ++) {
				var pid = y * width + x;
				resultPixels[pid * 4 + 0] = mapResult[i];
				resultPixels[pid * 4 + 1] = mapResult[i];
				resultPixels[pid * 4 + 2] = mapResult[i];
				resultPixels[pid * 4 + 3] = pixels[pid * 4 + 3];
			}
		}

		
	}
	
	/*=================================================================================*/
	return resultPixels;
}