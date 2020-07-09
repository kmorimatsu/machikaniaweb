/****************************************
*  PIC32MX emulator written by Katsumi  *
*        This script is released        *
*          under the LGPL v2.1.         *
****************************************/

fsfile=new Object();
/*
typedef struct
{
    unsigned long seek;
    unsigned long size;
    unsigned char flags;
    char          name[11];
} FSFILE;
*/
fsfile.seek=function(address){
	return system.read32(address);
};
fsfile.seek.set=function(address,value){
	system.write32(address,value);
};
fsfile.size=function(address){
	return system.read32(address+4);
};
fsfile.size.set=function(address,value){
	system.write32(address+4,value);
};
fsfile.flags=function(address){
	return system.read8(address+8);
};
fsfile.flags.set=function(address,value){
	system.write8(address+8,value);
};
searchrec=new Object();
/*
typedef struct
{
    unsigned long filesize;
    unsigned char attributes;   // 0 or ATTR_DIRECTORY(0x10)
    char          filename[13];
} SearchRec;
*/
searchrec.filesize=function(address){
	return system.read32(address);
};
searchrec.filesize.set=function(address,value){
	system.write32(address,value);
};
searchrec.attributes=function(address){
	return system.read8(address+4);
};
searchrec.attributes.set=function(address,value){
	system.write8(address+4,value);
};
searchrec.filename=function(address){
	var ret='';
	var i,c;
	for(i=0;i<13;i++){
		c=system.read8(address+9+i);
		if (0==c) break;
		ret+=String.fromCharCode(c);
	}
	return ret;
};
searchrec.filename.set=function(address,str){
	var i;
	str=str.toUpperCase();
	str=str.substr(0,12);
	for(i=0;i<str.length;i++){
		system.write8(address+5+i,str.charCodeAt(i));
	}
	system.write8(address+5+i,0);
};

filesystem=new Object();
filesystem.fhandle0=0;
filesystem.fhandle1=0;
filesystem.fhandle0dir=0;
filesystem.fhandle1dir=0;
filesystem.fhandle0file=0;
filesystem.fhandle1file=0;
filesystem.root=new Array();
filesystem.root["LIB"]=new Array();
filesystem.root["LIB"]["TEST"]="#TEST\n";
filesystem.root["MACHIKAM.INI"]="#\n#\n";
filesystem.root["TEST.BAS"]=(function(){/*cls
usegraphic 5
cursor 19,12
print "Hello World!"
circlefill 200,100,50,1
a$=input$()
*/}).toString().match(/\/\*([\s\S]*)\*\//)[1];
filesystem.root["TEST2.BAS"]=(function(){/*cls
usegraphic 9
cursor 32,12
print "Hello World!"
circlefill 300,100,50,1
a$=input$()
*/}).toString().match(/\/\*([\s\S]*)\*\//)[1];
filesystem.root["TEST3.BAS"]=(function(){/*cls
for i=1 to 1000
print i;
next
*/}).toString().match(/\/\*([\s\S]*)\*\//)[1];
filesystem.curdir=filesystem.root;
filesystem.curdirpath='\\';
filesystem.getDir=function(stream){
	if (this.fhandle0==stream) return this.fhandle0dir;
	if (this.fhandle1==stream) return this.fhandle1dir;
	return 0;
};
filesystem.getFile=function(stream){
	if (this.fhandle0==stream) return this.fhandle0file;
	if (this.fhandle1==stream) return this.fhandle1file;
	return 0;
};
filesystem.toString=function(pfilename){
	var ret='';
	var c;
	while(c=system.read8(pfilename++)){
		ret+=String.fromCharCode(c);
	}
	return ret;
};
filesystem.checkFileName=function(filename){
	var ret='';
	var i,c;
	filename=filename.toUpperCase();
	if (8<filename.length && -1==filename.indexOf('.')) {
		ret=filename.substr(0,8)+'.'+filename.substr(8);
	} else {
		ret=filename;
	}
	// Restrict length <=8
	i=ret.indexOf('.');
	if (i<0) {
		return ret.substr(0,8);
	} else if (8<i) {
		ret=ret.substr(0,8)+ret.substr(i);
	}
	i=ret.indexOf('.');
	// Restrict extension length <=3
	if (ret.length<=i+4) {
		return ret;
	} else {
		return ret.substr(0,i+4);
	}
};
filesystem.FSInit=function(){// TODO: around here (initiation).
	//int FSInit(void);
	//this.fhandle0=system.pGFileArray0;
	//this.fhandle1=system.pGFileArray1;
	this.fhandle0=0;
	this.fhandle1=0;
	return 1;
};
filesystem.FSfopen=function(fileName,mode){
	//FSFILE * FSfopen(const char * fileName, const char *mode);
	// Check file name
	fileName=this.checkFileName(this.toString(fileName));
	// Check file handle
	var fhandle=0;
	if (!this.fhandle0) {
		fhandle=system.pGFileArray0;
	} else if (!this.fhandle0) {
		fhandle=system.pGFileArray1;
	} else {
		// No more file handle
		return 0;
	}
	// Check if file exists
	var exists=(typeof this.curdir[fileName]==="undefined") ? 0:1;
	// Inialize parameters
	var flags=0;
	var size=0;
	var seek=0;
	switch(this.toString(mode).toLowerCase()){
		case 'a':
			if (!exists) this.curdir[fileName]='';
			size=this.curdir[fileName].length;
			seek=size;
			flags=0x01;
			break;
		case 'w':
			this.curdir[fileName]='';
			flags=0x01;
			break;
		case 'r':
			if (!exists) return 0;
			size=this.curdir[fileName].length;
			flags=0x02;
			break;
		case 'a+':
			if (!exists) this.curdir[fileName]='';
			size=this.curdir[fileName].length;
			seek=size;
			flags=0x03;
			break;
		case 'w+':
			this.curdir[fileName]='';
			flags=0x03;
			break;
		case 'r+':
			if (!exists) return 0;
			size=this.curdir[fileName].length;
			flags=0x03;
			break;
		default:
			// Invalid mode
			return 0;
	}
	// Update fhandle
	if (!this.fhandle0) {
		this.fhandle0=fhandle;
		this.fhandle0dir=this.curdir;
		this.fhandle0file=fileName;
	} else if (!this.fhandle0) {
		this.fhandle1=fhandle;
		this.fhandle1dir=this.curdir;
		this.fhandle1file=fileName;
	} else {
		// Unknown error
		return 0;
	}
	fsfile.size.set(fhandle,size);
	fsfile.seek.set(fhandle,seek);
	fsfile.flags.set(fhandle,flags);
	return fhandle;
};
filesystem.FSfclose=function(fo){
	//int FSfclose(FSFILE *fo);
	if (fo==this.fhandle0) this.fhandle0=0;
	if (fo==this.fhandle1) this.fhandle1=0;
	return 1;
};
filesystem.FSfeof=function(stream){
	//int FSfeof( FSFILE * stream );
	return fsfile.size(stream)<=fsfile.seek(stream) ? 1:0;
};
filesystem.FSfread=function(ptr,size,n,stream){
	//size_t FSfread(void *ptr, size_t size, size_t n, FSFILE *stream);
	// Prepare HTML5 conditions
	var dir=this.getDir(stream);
	var file=this.getFile(stream);
	if (!dir) return 0;
	if (fsfile.flags(stream) & 0x02) {
		// Prepare PIC32 conditions
		n=size*n;
		size=fsfile.size(stream);
		var seek=fsfile.seek(stream);
		if (size-seek<n) n=size-seek;
		fsfile.seek.set(stream,seek+n);
		// Update RAM in PIC32
		for(var i=0;i<n;i++){
			system.write8(ptr+i,dir[file].charCodeAt(seek+i));
		}
		return n;
	} else {
		// Reading is not allowed
		return 0;
	}
};
filesystem.FSfwrite=function(data_to_write,size,n,stream){
	//size_t FSfwrite(const void *data_to_write, size_t size, size_t n, FSFILE *stream);
	// Prepare HTML5 conditions
	var dir=this.getDir(stream);
	var file=this.getFile(stream);
	if (!dir) return 0;
	if (typeof dir[file]!='string') return 0;
	var str=dir[file];
	if (fsfile.flags(stream) & 0x01) {
		// Prepare PIC32 conditions
		n=size*n;
		size=fsfile.size(stream);
		var seek=fsfile.seek(stream);
		fsfile.seek.set(stream,seek+n);
		if (size<seek+n) size=seek+n;
		fsfile.size.set(stream,size);
		// Update string in HTML5
		var result=str.substring(0,seek);
		for(var i=0;i<n;i++){
			result+=String.fromCharCode(system.read8(data_to_write+i));
		}
		result+=str.substring(seek+n);
		dir[file]=result;
		return n;
	} else {
		// Writing is not allowed
		return 0;
	}
};
filesystem.FSftell=function(fo){
//long FSftell(FSFILE *fo);
alert('ftell');
};
filesystem.FSfseek=function(stream,offset,whence){
//int FSfseek(FSFILE *stream, long offset, int whence);
alert('fseek');
};
filesystem.FSrewind=function(fo){
//void FSrewind (FSFILE *fo);
alert('rewind');
};
filesystem.FindFirst=function(fileName,attr,rec){
	//int FindFirst (const char * fileName, unsigned int attr, SearchRec * rec);
	// Construct regular expression for detecting file(s) in directory
	var re,i,c;
	fileName=this.toString(fileName)
	// '*' -> [A-Z_]*
	// '?' -> [A-Z]
	re='';
	for(i=0;i<fileName.length;i++){
		c=fileName.charAt(i);
		switch(c){
			case '?':
				re+='[^\\."\\\\/\\[\\]\\:;\\s]';
				break;
			case '*':
				re+='[^\\."\\\\/\\[\\]\\:;\\s]*';
				break;
			case '.':
				re+='\\.';
				break;
			default:
				re+=c;
				break;
		}
	}
	re='^'+re+'$';
	re=new RegExp(re);
	// Checkout directory
	this.FindFirst.data=new Array();
	for(var file in this.curdir){
		if (!re.exec(file)) continue;
		if (attr & 0x10) {
			if (typeof this.curdir[file]=='object') this.FindFirst.data.push(file);
		} else {
			if (typeof this.curdir[file]=='string') this.FindFirst.data.push(file);
		}
	}
	if (attr & 0x10) {
		// Directories
		if (this.curdirpath!='\\') this.FindFirst.data.push('..');
		this.FindFirst.data.push('.');
	}
	// Goto FindNext function.
	return this.FindNext(rec);
};
filesystem.FindNext=function(rec){
//int FindNext (SearchRec * rec);
	if (typeof this.FindFirst.data !='object') return -1;
	if (!this.FindFirst.data.length) return -1;
	var file=this.FindFirst.data.pop();
	if (typeof this.curdir[file]=='object' || file=='.' || file=='..') {
		searchrec.attributes.set(rec,0x10);
		searchrec.filesize.set(rec,0);
	} else {
		searchrec.attributes.set(rec,0x00);
		searchrec.filesize.set(rec,this.curdir[file].length);
	}
	searchrec.filename.set(rec,file);
	return 0;
};
filesystem.FSmkdir=function(path){
//int FSmkdir (char * path);
alert('mkdir');
};
filesystem.FSgetcwd=function(path,numbchars){
//char * FSgetcwd (char * path, int numbchars);
	if (!path) return 0;
	if (numbchars<this.curdirpath.length) return 0;
	var i;
	for(i=0;i<this.curdirpath.length;i++){
		system.write8(path+i,this.curdirpath.charCodeAt(i));
	}
	system.write8(path+i,0);
	return path;
};
filesystem.FSchdir=function(path){
//int FSchdir (char * path);
	var i;
	// Determine absolute path
	path=this.toString(path);
	switch (path) {
		case '\\':
			break;
		case '.':
			path=this.curdirpath;
			break;
		case '..':
			i=this.curdirpath.lastIndexOf('\\');
			if (i<1) {
				path='\\';
			} else {
				path=this.curdirpath.substr(0,i);
			}
			break;
		default:
			path=this.curdirpath+'\\'+path;
			break;
	}
	if (path.substr(0,2)=='\\\\') path=path.substr(1);
	// Check if the new path exists
	var newpath=this.root;
	var temppath=path;
	if (path=='\\') {
		// Done
	} else {
		temppath=path.substr(1)+'\\';
		while(1<temppath.length){
			i=temppath.indexOf('\\');
			if (typeof newpath[temppath.substr(0,i)]=="object") {
				newpath=newpath[temppath.substr(0,i)];
				temppath=temppath.substr(i+1);
			} else {
				newpath=0;
				break;
			}
		}
	}
	// All done
	if (newpath) {
		this.curdir=newpath;
		this.curdirpath=path;
		return 0;
	} else {
		// Failed
		return -1;
	}
};
filesystem.FSremove=function(fileName){
	//int FSremove (const char * fileName);
	// Check file name
	fileName=this.checkFileName(this.toString(fileName));
	if (typeof this.curdir[fileName]=='string') {
		delete this.curdir[fileName];
		return 0;
	} else {
		return -1;
	}
};
filesystem.FSrename=function(fileName,fo){
//int FSrename (const char * fileName, FSFILE * fo);
alert('rename');
};
/*
	Disk system - Zip archive handling routines follow
*/
filesystem.saveLink=function(obj){
	// Construct ZIP archive
	var zip = new JSZip();
	// Construct recursive function
	var explore=function(dir,zip){
		var file;
		for(file in dir){
			if (Array.isArray(dir[file])) {
				explore(dir[file],zip.folder(file));
			} else {
				zip.file(file,dir[file]);
			}
		}
	};
	// Construct ZIP object
	explore(this.root,zip);
	// Prepare to download
	var data="data:application/zip;base64,";
	data+=zip.generate({type:"base64",compression: "DEFLATE"});
	// Update href property of a tag
	obj.href=data;
	obj.click();
};
filesystem.setFile=function(obj){
	// Show the dialog to upload local file.
	obj.click();
};
filesystem.loaded=function(obj){
	// This will be called when a file is uploaded.
	// If FileReader API is not supported, following code will fail.
	var fr = new FileReader();
	fr.onload = function () {
		var data=new Uint8Array(fr.result);
		filesystem.update(data);
	};
	this.name=obj.files[0].name;
	fr.readAsArrayBuffer(obj.files[0]);
};
filesystem.update=function(data){
	// This will be called when a file is sucessfully loaded by FileReader API.
	// Data will be given as an array
	// Check if ZIP archive.
	if (data[0]==0x50 && data[1]==0x4B && data[2]==0x03 && data[3]==0x04) {
		this.loadZip(new JSZip(data));
	} else {
		alert("Not Zip archive!");
	}
};
filesystem.loadZip=function(zip){
	var file,m;
	// Construt empty disk
	var root=new Array();
	// Function to explore ZIP archive
	var explore=function(filename,dir,contents){
		filename=filename.toUpperCase();
		m=filename.match(/^([^\/]+)\/(.*)$/);
		if (m) {
			if (!Array.isArray(dir[m[1]])) dir[m[1]]=new Array();
			if (m[2].length) explore(m[2],dir[m[1]],contents);
		} else {
			dir[filename]=contents.asText();
		}
	};
	// List up all files in ZIP archive and store in the disk image
	for(file in zip.files){
		explore(file,root,zip.file(file));
	}
	// Replace disk image in file system
	this.root=root;
};