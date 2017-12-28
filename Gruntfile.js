module.exports = function(grunt){
    grunt.initConfig({
        pkg:grunt.file.readJSON('package.json'),

        watch:{
            options:{livereload:true},
            files:['public/**','server/**'],
            tasks:[]
        },
        express:{
            all:{
                options:{
                    server:'server.js',
                    hostname:'localhost',
                    bases:['./public'],
                    livereload:true
                }
            }
        },
        'gh-pages': {
            options: {
                base: 'public'
            },
          src: ['**']
        }
    });

    grunt.loadNpmTasks('grunt-contrib-watch');
    grunt.loadNpmTasks('grunt-express');
    grunt.loadNpmTasks('grunt-gh-pages');
    grunt.registerTask('server',['express','watch']);
    grunt.registerTask('deploy', ['gh-pages']);

};

