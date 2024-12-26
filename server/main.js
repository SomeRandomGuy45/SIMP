const express = require('express')
const path = require('path')
const app = express()
const port = 8080

const defaultMessage = { status: 'OK', port: port }
const projectDataDir = path.join(__dirname, 'projects')

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
        url : `api/project-data/${projectName}/download`
    }

    res.setHeader('Content-Type', 'application/json')
    res.json(projectData)
})

app.get('/api/project-data/:projectName/download', (req, res) => {
    const projectName = req.params.projectName
    const filePath = path.join(projectDataDir, `${projectName}.zip`)

    // Check if the file exists
    res.sendFile(filePath, { 
        headers: {
            'Content-Disposition': `attachment; filename="${projectName}.zip"`  // Ensures the filename is set in the download
        }
    }, (err) => {
        if (err) {
            // Only handle the error here without trying to send another response
            if (!res.headersSent) {
                res.status(404).json({ status: 'Error', message: 'File not found' })
            }
        }
    })
})

app.listen(port, () => {
    console.log(`Server listening on port ${port}`)
    console.log(`Default message: ${JSON.stringify(defaultMessage)}`)
})