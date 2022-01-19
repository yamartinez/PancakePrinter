const express = require("express")
const fileUpload = require("express-fileupload")
const {exec} = require("child_process")
const fs = require("fs")

const app = express();

async function Log(content){
    fs.writeFile('./Log.txt', content+"\n", { flag: 'a+' }, err => {})
}

app.use(express.static("./public"))

app.use(fileUpload({
    useTempFiles:true,
    tempFileDir:"./tmp/",
    preserveExtension:true
}))

async function processImage(path,res){
    exec(`./uart_test.py ./paths/${path}.path`,(err, stdout, stderr)=>{
        if(err){
            console.error(err)
            Log(err)
        }
        console.log(stdout)
    }).on('close',(e)=>{
        res.sendStatus(200)
    })
}

async function skeletonize(path,res){
    let sent = false
    fname = path.split(".")[0]
    fext = path.split(".")[1]
    exec(`./process_images.py ./uploads/${path} ./public/processed/${fname}_1.${fext} ./public/processed/${fname}_2.${fext} ./public/processed/${fname}_3.${fext} ./public/processed/${fname}_4.${fext} ./public/processed/${fname}_5.${fext} ./paths/${fname}_1.${fext}.path ./paths/${fname}_2.${fext}.path ./paths/${fname}_3.${fext}.path ./paths/${fname}_4.${fext}.path ./paths/${fname}_5.${fext}.path`
    ,(err,stdout,stderr)=>{
        if(err){
            console.error(err)
            res.sendStatus(500)
            Log(err)
            sent = true
        }
        if(stdout)
            console.log(stdout)
        if(stderr)
            console.error(stderr)

    }).on("close",(e)=>{
        if(!sent)
            res.sendStatus(200)  
    })
}

app.post("/upload", async(req, res) => {
    console.log("Upload received")
    let image = req.files.image
    imagepath = `./uploads/${image.name}`
    image.mv(imagepath)
    skeletonize(image.name,res)
    // res.send("File Uploaded")
    // processImage(image.name)

})

app.post("/print/:file",async(req,res)=>{
    let path = req.params.file
    processImage(path,res)
    // console.log(path)
    // res.sendStatus(200)
})

app.listen(8080)