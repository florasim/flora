var path = require('path');

    module.exports = function(app) {

       // basic routes to handle request
        app.get('/', function(req, res) {
                console.log("Main route");
                res.sendFile(path.join(__dirname, '../../public', 'index.html'));

        });

    };

