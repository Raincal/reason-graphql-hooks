const bsconfig = require('./bsconfig.json');
const withCSS = require('@zeit/next-css');
const withTM = require('next-transpile-modules')(['bs-platform'].concat(bsconfig['bs-dependencies']));

const config = {
  target: 'serverless',
  pageExtensions: ['jsx', 'js', 'bs.js']
};

module.exports = withTM(withCSS(config));

