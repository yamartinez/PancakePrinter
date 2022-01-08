const express = require("express")
const fileUpload = require("express-fileupload")
const {exec} = require("child_process")

const app = express();

app.use(express.static("/home/pi/server/public"))

app.use(fileUpload({
    useTempFiles:true,
    tempFileDir:"./tmp/",
    preserveExtension:true
}))

async function processImage(path){
    exec(`./program.py /home/pi/server/uploads/${path} /home/pi/server/paths/${path}.path`,(err, stdout, stderr)=>{
        if(err){
            console.error(err)
        }
        console.log(stdout)
    })
}

app.post("/infill", async(req, res) => {
    exec(`./uart_test.py /home/pi/server/paths/INFILL.path`,(err,stdout,stderr)=>{
        if(err){
        console.error(err)
        }
        console.log(stdout)
    });
    res.send("Done")
})

app.post("/upload", async(req, res) => {
    console.log("Upload received")
    let image = req.files.image
    imagepath = `/home/pi/server/uploads/${image.name}`
    image.mv(imagepath)
    res.send("File Uploaded")
    processImage(image.name)
})

app.listen(8080)
