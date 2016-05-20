/**
 * Welcome to Pebble.js!
 *
 * This is where you write your app.
 */

var UI = require('ui');

var main = new UI.Card({
  status: {
    color: 'white',
    backgroundColor: 'sunset-orange',
    seperator:'dotted'
  },
  title: 'Chord Pusher',
  subtitle: 'Push a button!.',
  body: 'Up: Dm, Select: Bm, Down: Gm',
});

main.show();

main.on('click', 'up', function(e) {
var ajax = require('ajax');

  ajax({ url: 'http://iesc-s2.mit.edu/student_code/matiash/dev1/sb2.py', method: 'post', data: {username:'sarah', password:'loves_uke', action: 'send-pattern', 'song-title':'Pebble Song',index:0}},
  function() {
    main.body('Sent the Dm chord!');
    main.show();
  }
);

});

main.on('click', 'select', function(e) {
  var ajax = require('ajax');

  ajax({ url: 'http://iesc-s2.mit.edu/student_code/matiash/dev1/sb2.py', method: 'post', data: {username:'sarah', password:'loves_uke', action: 'send-pattern', 'song-title':'Pebble Song',index:1}},
  function() {
    main.body('Sent the Bm chord!');
    main.show();
  }
);
});

main.on('click', 'down', function(e) {
  var ajax = require('ajax');

  ajax({ url: 'http://iesc-s2.mit.edu/student_code/matiash/dev1/sb2.py', method: 'post', data: {username:'sarah', password:'loves_uke', action: 'send-pattern', 'song-title':'Pebble Song',index:2}},
  function() {
    main.body('Sent the Gm chord!');
    main.show();
  }
);
});
