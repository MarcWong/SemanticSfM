var express = require('express');
var router = express.Router();
var multer  = require('multer')
const fs = require('fs');

/* GET home page. */
router.get('/result', function(req, res, next) {
	res.render('result', { title: 'Express',results : '/javascripts/robust_colorized_semantic.ply'});
});

router.get('/',function(req, res, next) {
	res.render('upload', { title: 'Express' });
});

router.get('/analysis',function(req, res, next) {
	console.log('analysis')
	var exec = require('child_process').exec,child;
	child = exec('../SementicSFM/do.sh ./public/uploads ./public/uploads',function(err,out) { 
		console.log(out); err && console.log(err); 
		res.json({res_code: '0'});
	});
	res.json({ success: 'success' });
});

var storage = multer.diskStorage({
    destination: function (req, file, cb) {
        cb(null, './public/uploads')
    },
    filename: function (req, file, cb) {
        var fileformat = (file.originalname).split('.');
        cb(null, file.fieldname+'-'+Date.now()+'.'+fileformat[fileformat.length-1]);
    }
})

var upload = multer({ storage: storage })

router.post('/upload',upload.any(),function(req,res,next){
	console.log(req.files.length);
    res.json({res_code: '0'});
});

function deleteall(path) {
	var files = [];
	if(fs.existsSync(path)) {
		files = fs.readdirSync(path);
		files.forEach(function(file, index) {
			var curPath = path + "/" + file;
			if(fs.statSync(curPath).isDirectory()) { // recurse
				deleteall(curPath);
			} else { // delete file
				fs.unlinkSync(curPath);
			}
		});
		fs.rmdirSync(path);
	}
};

router.post('/clean',function(req,res,next){
	console.log('clean')
	var exec = require('child_process').exec,child;
	child = exec('rm -rf ./public/uploads/*',function(err,out) { 
		console.log(out); err && console.log(err); 
		res.json({res_code: '0'});
	});
	
});
module.exports = router;
