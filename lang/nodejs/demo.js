var j;
var i;
var fs = require ('fs');
var sr = [0x85,0xC0,0x74,0x08,0x33,0xDB,0x43];
var fn = "sublime_text.exe";

//对文件进行操作
fs.readFile(fn,function(err,data){
	if (err) throw err;
	console.log(data.length,sr.length);
	for (i = 0;i< data.length; i++) {
		if (data[i]==sr[0]){
			for (j = 1;j<sr.length; j++) {
				if (data[j+i]!=sr[j]) {
					j=-1;
					break;
				};
			};
			
			if (j==sr.length){
				console.log("Found:",j+i-1,data[j+i-1]);
				data[j+i-1] = 0x90;
				fs.writeFile("x"+fn,data,function(err){
					if(err) throw err;
					console.log('Crack successfully!');
				})
			}
		}
	};
});


