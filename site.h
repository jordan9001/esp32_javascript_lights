#ifndef SITE_H
#define SITE_H

#define SITESTART	\
"<html>\n"\
"        <head>\n"\
"                <title>Esp Lights</title>\n"\
"                <style type='text/css'>\n"\
"                        #editor {\n"\
"                                margin: auto;\n"\
"                                position: absolute;\n"\
"                                top: 0;\n"\
"                                right: 0;\n"\
"                                bottom: 0;\n"\
"                                left: 0;\n"\
"                                font-size: 18px;\n"\
"                        }\n"\
"                        #submit {\n"\
"                                margin: auto;\n"\
"                                position: absolute;\n"\
"                                top: 80%;\n"\
"                                right: 0;\n"\
"                                left: 0;\n"\
"                                width: 80%;\n"\
"                                height: 10%;\n"\
"                                text-align: center;\n"\
"                                font-size: 33px;\n"\
"                        }\n"\
"                </style>\n"\
"                <script src='https://cdnjs.cloudflare.com/ajax/libs/ace/1.4.7/ace.js' integrity='sha256-C7DTYRJLG+B/VEzHGeoPMw699nsTQYPAXHKXZb+q04E=' crossorigin='anonymous'></script>\n"\
"        </head>\n"\
"        <body>\n"\
"                <div id='editor'>\n"\



#define SITEEND		\
"                </div>\n"\
"                <button id='submit' type='button'>Submit</button>\n"\
"                <script type='text/javascript'>\n"\
"var editor = ace.edit('editor');\n"\
"editor.setTheme('ace/theme/monokai');\n"\
"editor.session.setMode('ace/mode/javascript');\n"\
"// set button to do a post of editor.getSession().getDocument().getAllLines()\n"\
"var btn = document.getElementById('submit');\n"\
"btn.onclick = function() {\n"\
"        var xhr = new XMLHttpRequest();\n"\
"        xhr.open('POST', '/update', 'true');\n"\
"        var lines = editor.getSession().getDocument().getAllLines().join('\\n');\n"\
"        xhr.send(lines);\n"\
"};\n"\
"                </script>\n"\
"        </body>\n"\
"</html>\n"\



#endif /* SITE_H */
