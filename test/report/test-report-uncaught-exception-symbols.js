'use strict';
// Test producing a report on uncaught exception.
const common = require('../common');
const assert = require('assert');
const childProcess = require('child_process');
const helper = require('../common/report');
const tmpdir = require('../common/tmpdir');

if (process.argv[2] === 'child') {
  throw Symbol('foobar');
}

tmpdir.refresh();
const child = childProcess.spawn(process.execPath, [
  '--report-uncaught-exception',
  __filename,
  'child',
], {
  cwd: tmpdir.path,
});
child.on('exit', common.mustCall((code) => {
  assert.strictEqual(code, 1);
  const reports = helper.findReports(child.pid, tmpdir.path);
  assert.strictEqual(reports.length, 1);

  helper.validate(reports[0], [
    ['header.event', 'Exception'],
    ['header.trigger', 'Exception'],
    ['javascriptStack.message', 'Symbol(foobar)'],
  ]);
}));
