const express = require('express')
const app = express()
const port = 8080

const defaultMessage = { status: 'OK', port: port }

app.get('/', (req, res) => {
    res.setHeader('Content-Type', 'application/json')
    res.json(defaultMessage)
})

app.get('/api/', (req, res) => {
    res.setHeader('Content-Type', 'application/json')
    res.json(defaultMessage)
})

app.get('/api/project-data/:projectName', (req, res) => {
    const projectName = req.params.projectName
    const projectData = {
        status: 'OK',
        projectName: projectName,
        message: `Data for project: ${projectName}`
    }

    res.setHeader('Content-Type', 'application/json')
    res.json(projectData)
})

app.listen(port, () => {
    console.log(`Server listening on port ${port}`)
    console.log(`Default message: ${JSON.stringify(defaultMessage)}`)
})