const express = require("express")
const express_ws = require("express-ws")
const axios = require('axios')
const dotenv = require('dotenv');
const app = express()
const URLS = {
    0 : "http://localhost:8080/",
    1 : "",
    2 : "",
    3 : "",
    4 : "",
    5 : "",
}
const config = {
    PORT : process.env["PORT"] || 1010
}

let url = ""

express_ws(app)

dotenv.config()

async function checkURL(url__) {
    try {
        const response = await axios.get(url__)
        if (response.status == 200) {
            return true
        }
    } catch (error) {
        return false
    }
}

async function getDataFromURL(__url, __method, __headers, __data) {
    try {
        const response = await axios({
            url : __url,
            method : __method || 'get',
            headers : __headers || {},
            data : __data || {},
        })
        return response.data
    } catch (error) {
        console.error(error)
        return undefined
    }
}

app.ws('/', (ws, req) => {
    ws.on('message', (msg) => {
        ws.send('alive')
    })
})

app.ws('/register', async (ws, req) => {
    ws.on('message', async (msg) => {
        const headers = JSON.parse(msg)
        const username = headers['username']
        const password = headers['password']
        if (!password || !username) {
            console.log('no username || password')
            ws.close(400, 'Unable to register! Reason: No password or username.')
            return
        }
        await getDataFromURL(url + "register", 'post', { "Content-Type": "application/json" } , msg)
    })
})

app.ws('/login', async (ws, req) => {
    ws.on('message', async (msg) => {
        const headers = JSON.parse(msg)
        const username = headers['username']
        const password = headers['password']
        if (!password || !username) {
            console.log('no username || password')
            ws.close(400, 'Unable to register! Reason: No password or username.')
            return
        }
        const data = await getDataFromURL(url + "login", 'post', { "Content-Type": "application/json" } , msg)
        ws.send(data["token"])
    })
})

app.listen(config.PORT, () => {
    console.log("started to listen on port:",config.PORT)
    for (i = 0; i < 6; i++) {
        if (URLS[i] == "") continue
        let boolCheck = checkURL(URLS[i])
        if (boolCheck) {
            url = URLS[i]
            console.log("using url:", url)
            break
        }
    }
})