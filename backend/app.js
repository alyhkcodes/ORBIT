require('dotenv').config();

const connectDB = require('./db/connect');
const express = require('express');
const app = express();


const port = process.env.PORT || 3000;

const start = async () => {
    try {
        await connectDB(process.env.MONGO_URI);
        app.listen(port, () => console.log(`Server listening on port ${port}...`));
    } catch (error) {
        console.error('Failed to start server:', error.message);
        process.exit(1);
    }
};

start();