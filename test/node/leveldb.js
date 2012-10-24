var leveldb = require('leveldb');

leveldb.open("../wireshark/ws.db", { create_if_missing: false }, onOpen);

function onOpen(err, db) {
	var key = "mykey";
	db.put(key, "My Value!", function(err) {
		db.get(key, function(err, value) {
			console.dir(value); // prints: My Value!
			db.del(key);
		});
	});
}