var webpack = require('webpack');
var WebpackDevServer = require('webpack-dev-server');
var config = require('./webpack.config');

const devServerConfig = config.devServer;
const devServerDefaultConfig = {
  publicPath: config.output.publicPath,
  hot: true,
  historyApiFallback: true,
  // It suppress error shown in console, so it has to be set to false.
  quiet: false,
  // It suppress everything except error, so it has to be set to false as well
  // to see success build.
  noInfo: false,
  stats: {
    // Config for minimal console.log mess.
    assets: false,
    colors: true,
    version: false,
    hash: false,
    timings: false,
    chunks: false,
    chunkModules: false
  }
};

const devServerOptions = {};

Object.keys(devServerDefaultConfig).forEach((k) => {
  devServerOptions[k] = devServerDefaultConfig[k];
});

Object.keys(devServerConfig).forEach((k) => {
  devServerOptions[k] = devServerConfig[k];
});

new WebpackDevServer(webpack(config), devServerOptions)
  .listen(3000, 'localhost', function (err) {
    if (err) {
        console.log(err);
    }

  console.log('Listening at localhost:3000');
});