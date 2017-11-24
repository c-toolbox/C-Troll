## Run the app in development
Edit `backend/config.json` to configure ports and addresses.

Start the webpack dev server:

- ```npm install```
- ```npm start```

Start the backend server (connects web backend to core)

- ```cd backend```
- ```npm install```
- ```npm start```

Visit  ```localhost:<port> ``` in a browser

## Build the app
- ```npm run build``` (Bundles the whole web application into the `dist` folder.)

Distribute the contents of the dist folder.

## Run the app in production
Edit `dist/config.json` to configure ports and addresses.
Run the ```dist/server``` executable.

Visit ```localhost:<port> ``` in a browser
