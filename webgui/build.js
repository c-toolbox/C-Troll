const os = require('os');
const enclose = require('enclose').exec;
const cp = require('cp');
const rimraf = require('rimraf');
const mkdirp = require('mkdirp');
const webpack = require('webpack');

const ProgressBarPlugin = require('progress-bar-webpack-plugin');
const webpackConfig = require('./webpack.production.config');

const executable = {
    win32: 'server.exe',
}[os.platform()] || 'server';

rimraf('dist', () => {
    mkdirp('dist', () => {
        const compiler = webpack(webpackConfig);

        compiler.apply(new ProgressBarPlugin({
            format: '  build [:bar] :percent (:elapsed seconds)',
            clear: false
        }));

        compiler.run(() => {
            cp.sync('backend/config.json', 'dist/config.json');
        });

        enclose(['--output', 'dist/' + executable, 'backend/server.js']);
    });
});
