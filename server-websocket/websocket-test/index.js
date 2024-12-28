const loginButton = document.getElementById("login_button")
const regButton = document.getElementById("register_button")
const input_text_password = document.getElementById("password")
const input_text_name = document.getElementById("username")

function regButtonLogic() {
    const headers = {username : input_text_name.value, password : input_text_name.value}
    const websocket = new WebSocket("ws://localhost:1010/register")
    websocket.addEventListener("open", (e) => {
        websocket.send(JSON.stringify(headers))
    })
}

function loginButtonLogic() {
    const headers = {username : input_text_name.value, password : input_text_name.value}
    const websocket = new WebSocket("ws://localhost:1010/login")
    websocket.addEventListener("open", (e) => {
        websocket.send(JSON.stringify(headers))
    })
    websocket.addEventListener("message", (e) => {
        console.log(e)
    })
}

regButton.addEventListener("click", (e) => {
    e.preventDefault()
    regButtonLogic()
})

loginButton.addEventListener("click", (e) => {
    e.preventDefault()
    loginButtonLogic()
})