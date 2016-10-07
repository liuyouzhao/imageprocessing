var gradient = function() {}

gradient.process = function(pixels, width, height, dirv) {
	return __gradient_process(pixels, width, height, dirv);
}
/*
* Main processing function
*/
function __gradient_process(pixels, width, height, dirv) {

	var resultPixels = [];
	/* prewitt */
	var prewittX = [
		-1, 0, 1,
		-1, 0, 1,
		-1, 0, 1
	];

	var prewittY = [
		-1, -1, -1,
		 0, 0, 0,
		 1, 1, 1
	];
	/* 3 X 3 */
	var preW = 3;

    /*===============================================================================*/
	/* Our teacher said that all things are some "for" loop, here it is
	 * Change the code below, with your knowledge from books.
	*/
	for(var i = 0, j = 0; i < height * width * 4; i += 4, j ++) {

		
		var redX = 0, greenX = 0, blueX = 0;
		var redY = 0, greenY = 0, blueY = 0;
		
		for(var y = -1; y <= 1; y ++) {
			for(var x = -1; x <= 1; x ++) {

				var curRow = Math.floor(j / width) + y;
				var curCol = j % width + x;
				curRow = curRow < 0 ? 0 : curRow;
				curCol = curCol < 0 ? 0 : curCol;
				curRow = curRow >= height ? height - 1 : curRow;
				curCol = curCol >= width ? width - 1 : curCol;

				var curIndex = curRow * width + curCol;
				var prewittI = (y + 1) * preW + x + 1;

				redX += pixels[curIndex * 4] * prewittX[prewittI];
				greenX += pixels[curIndex * 4 + 1] * prewittX[prewittI];
				blueX += pixels[curIndex * 4 + 2] * prewittX[prewittI];

				redY += pixels[curIndex * 4] * prewittY[prewittI];
				greenY += pixels[curIndex * 4 + 1] * prewittY[prewittI];
				blueY += pixels[curIndex * 4 + 2] * prewittY[prewittI];
			}
		}

		if(dirv) {
			resultPixels[i] =  Math.atan(redX / redY) * 180 / Math.PI + 90;
			resultPixels[i + 1] = Math.atan(greenX / greenY) * 180 / Math.PI + 90;
			resultPixels[i + 2] = Math.atan(blueX / blueY) * 180 / Math.PI + 90;
			resultPixels[i + 3] = pixels[i + 3];
		}
		else {
			resultPixels[i] = Math.sqrt(redX*redX + redY*redY);
			resultPixels[i + 1] = Math.sqrt(greenX*greenX + greenY*greenY);
			resultPixels[i + 2] = Math.sqrt(blueX*blueX + blueY*blueY);
			resultPixels[i + 3] = pixels[i + 3];
		}
	}

	/*=================================================================================*/
	return resultPixels;
}