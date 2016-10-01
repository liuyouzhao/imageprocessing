var convolute = function() {}

convolute.process = function(pixels, width, height, opera, ow, fix) {
	return __convolute_process(pixels, width, height, opera, ow, fix);
}
/*
* Main processing function
*/
function __convolute_process(pixels, width, height, opera, ow, fix) {

	var resultPixels = [];

	/* w X w */
	var hw = Math.floor(ow / 2);

    /*===============================================================================*/
	/* Our teacher said that all things are some "for" loop, here it is
	 * Change the code below, with your knowledge from books.
	*/
	for(var i = 0, j = 0; i < height * width * 4; i += 4, j ++) {

		
		var red = 0, green = 0, blue = 0;
		
		for(var y = -hw; y <= hw; y ++) {
			for(var x = -hw; x <= hw; x ++) {

				var curRow = Math.floor(j / width) + y;
				var curCol = j % width + x;
				curRow = curRow < 0 ? 0 : curRow;
				curCol = curCol < 0 ? 0 : curCol;
				curRow = curRow >= height ? height - 1 : curRow;
				curCol = curCol >= width ? width - 1 : curCol;

				var curIndex = curRow * width + curCol;
				var operaI = (y + hw) * ow + x + hw;

				red += pixels[curIndex * 4] * opera[operaI];
				green += pixels[curIndex * 4 + 1] * opera[operaI];
				blue += pixels[curIndex * 4 + 2] * opera[operaI];
			}
		}

		resultPixels[i] = red * fix;
		resultPixels[i + 1] = green * fix;
		resultPixels[i + 2] = blue * fix;
		resultPixels[i + 3] = pixels[i + 3];

	}

	
	/*=================================================================================*/
	return resultPixels;
}