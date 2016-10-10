var fs=require('fs');
var path = require('path');

var src = null;

var cmd = "../../bin/Debug/imgproc";

process.argv.forEach(function (val, index, array) {
    if(index == 2) src = val;
});



function ls(ff)
{
    var files=fs.readdirSync(ff);
    var i = 0;
    for(fn in files)
    {
        var fname = ff+path.sep+files[fn];
	var stat = fs.lstatSync(fname);
        if(stat.isDirectory() == true)
        {
            ls(fname);
        }
        else
        {
            
            //console.log(fname);
            fs.rename(fname, ff + "/" + (i + 1) + ".bmp");
        }
        i ++;
    }
}

ls(src);
