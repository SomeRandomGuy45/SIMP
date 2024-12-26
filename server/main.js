const express = require('express');
const path = require('path');
const multer = require('multer');
const fs = require('fs');
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const app = express();
const port = 8080;

const defaultMessage = { status: 'OK', port: port };
const projectDataDir = path.join(__dirname, 'projects');
const JWT_SECRET = 'Pizhxlz0ipjdxSJM8POCaET2gtRasCJ6';  // Secret key for signing JWT tokens

if (!fs.existsSync(projectDataDir)) {
    fs.mkdir(projectDataDir, (err, data) => {});
}
if (!fs.existsSync(path.join(__dirname, 'uploads.json'))) {
    fs.writeFile(path.join(__dirname, 'uploads.json'), '{}', (err) => {
        if (err) throw "Error creating folder";
        console.log('Created uploads.json');
    })
}
if (!fs.existsSync(path.join(__dirname, 'users.json'))) {
    fs.writeFile(path.join(__dirname, 'users.json'), '[]', (err) => {
        if (err) throw "Error creating folder";
        console.log('Created users.json');
    })
}

// Function to read users from users.json
function readUsersFromFile() {
    const filePath = path.join(__dirname, 'users.json');
    try {
        const data = fs.readFileSync(filePath, 'utf8');
        console.log(data)
        return JSON.parse(data);
    } catch (err) {
        console.error('Error reading users from file:', err);
        return [];
    }
}

// Function to save users to users.json
function saveUsersToFile(users) {
    const filePath = path.join(__dirname, 'users.json');
    try {
        const data = JSON.stringify(users, null, 2);
        fs.writeFileSync(filePath, data, 'utf8');
    } catch (err) {
        console.error('Error saving users to file:', err);
    }
}

// Function to read userUploads from uploads.json
function readUploadsFromFile() {
    const filePath = path.join(__dirname, 'uploads.json');
    try {
        const data = fs.readFileSync(filePath, 'utf8');
        return JSON.parse(data);
    } catch (err) {
        console.error('Error reading uploads from file:', err);
        return {};
    }
}

// Function to save userUploads to uploads.json
function saveUploadsToFile(uploads) {
    const filePath = path.join(__dirname, 'uploads.json');
    try {
        const data = JSON.stringify(uploads, null, 2);
        fs.writeFileSync(filePath, data, 'utf8');
    } catch (err) {
        console.error('Error saving uploads to file:', err);
    }
}

// Set up storage engine with multer
const storage = multer.diskStorage({
    destination: (req, file, cb) => {
        const projectName = req.params.projectName;
        const projectDir = path.join(projectDataDir, projectName);

        // Create project directory if it doesn't exist
        if (!fs.existsSync(projectDir)) {
        fs.mkdirSync(projectDir);
        }

        cb(null, projectDir); // Save to the project directory
    },
    filename: (req, file, cb) => {
        cb(null, file.originalname); // Use the original file name
    },
});

const upload = multer({ storage: storage });

// Parse JSON request bodies
app.use(express.json());

// Function to verify JWT token
function verifyToken(req, res, next) {
    const token = req.header('Authorization')?.replace('Bearer ', '');
    if (!token) {
        return res.status(401).json({ status: 'Error', message: 'Access denied, token missing' });
    }

    jwt.verify(token, JWT_SECRET, (err, decoded) => {
        console.log(token);
        if (err) {
        return res.status(401).json({ status: 'Error', message: 'Invalid token' });
        }
        req.userId = decoded.userId;  // Attach user ID to the request
        next();
    });
}

// Mock registration route (POST to register a new user)
app.post('/register', (req, res) => {
    const { username, password } = req.body;

    // Read users from the JSON file
    const users = readUsersFromFile();

    // Check if username is already taken
    const existingUser = users.find(u => u.username === username);
    if (existingUser) {
        return res.status(400).json({ status: 'Error', message: 'Username already taken' });
    }

    // Hash the password
    const hashedPassword = bcrypt.hashSync(password, 10);

    // Create the new user and add to the "database"
    users.push({ username, password: hashedPassword });

    // Save the updated users list to the file
    saveUsersToFile(users);

    res.json({ status: 'OK', message: 'User registered successfully' });
});

// Login route (POST to log in and generate JWT token)
app.post('/login', (req, res) => {
    const { username, password } = req.body;
    console.log(`Login user: ${username}`);

    // Read users from the JSON file
    const users = readUsersFromFile();

    // Find user by username
    const user = users.find(u => u.username === username);
    if (!user) {
        return res.status(401).json({ status: 'Error', message: 'Invalid username or password' });
    }

    // Compare password using bcrypt
    bcrypt.compare(password, user.password, (err, result) => {
        if (err || !result) {
        return res.status(401).json({ status: 'Error', message: 'Invalid username or password' });
        }

        // Generate JWT token
        const token = jwt.sign({ userId: user.username }, JWT_SECRET, { expiresIn: '100y' });

        res.json({ status: 'OK', message: 'Logged in successfully', token });
    });
});

app.post('/logout', (req, res) => {
    res.json({ status: 'OK', message: 'Logged out successfully' });
});

app.get('/', (req, res) => {
    res.setHeader('Content-Type', 'application/json');
    res.json(defaultMessage);
});

app.get('/api/', (req, res) => {
    res.setHeader('Content-Type', 'application/json');
    res.json(defaultMessage);
});

app.get('/api/project-data/:projectName', (req, res) => {
    const projectName = req.params.projectName;
    const projectData = {
        status: 'OK',
        projectName: projectName,
        url: `api/project-data/${projectName}/download`,
    };

    res.setHeader('Content-Type', 'application/json');
    res.json(projectData);
});

app.get('/api/project-data/:projectName/download', (req, res) => {
    const projectName = req.params.projectName;
    const filePath = path.join(projectDataDir, `${projectName}.zip`);

    // Check if the file exists
    res.sendFile(filePath, {
        headers: {
        'Content-Disposition': `attachment; filename="${projectName}.zip"`,
        },
    }, (err) => {
        if (err) {
        if (!res.headersSent) {
            res.status(404).json({ status: 'Error', message: 'File not found' });
        }
        }
    });
});

// Store uploaded file information with user ID
app.post('/api/project-data/:projectName/upload', verifyToken, upload.single('file'), (req, res) => {
    const projectName = req.params.projectName;
    const file = req.file;
    const userId = req.userId;  // Get user ID from token

    if (!file) {
        return res.status(400).json({ status: 'Error', message: 'No file uploaded' });
    }

    // Read userUploads from uploads.json
    const userUploads = readUploadsFromFile();

    // Check if the user has already uploaded a file for this project
    if (userUploads[projectName] && userUploads[projectName] !== userId) {
        return res.status(403).json({ status: 'Error', message: 'You cannot re-upload this file' });
    }

    // Record the user's upload
    userUploads[projectName] = userId;

    // Save the updated userUploads to uploads.json
    saveUploadsToFile(userUploads);

    res.setHeader('Content-Type', 'application/json');
    res.json({
        status: 'OK',
        message: `File ${file.originalname} uploaded successfully to project ${projectName}`,
        file: file,
    });
});

app.listen(port, () => {
    console.log(`Server listening on port ${port}`);
    console.log(`Default message: ${JSON.stringify(defaultMessage)}`);
});