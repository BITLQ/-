#!/usr/bin/env python
# coding=utf-8


class HtmlOutputer(object):
    def __init__(self):
        self.datas = []

    def collect_data(self,data):
        if data is None:
            return
        self.datas.append(data)

    def output_html(self):
        fout = open('/home/test/http_protect/output/wwwroot/weather_python/output.html','w')

        fout.write("<html><head><meta charset='utf-8'><head>")
        fout.write("<body>")
        fout.write("<table border='1'cellspacing='0'cellpadding='0'>")
        print "<html><head><meta charset='utf-8'><head>"
        print "<body>"
        print "<table border='1'cellspacing='0'cellpadding='0'>"

        for data in self.datas:
            fout.write("<tr>")
       #     fout.write("<td>{0}</td>".format(data['url'].encode('utf-8')))
            fout.write("<td>{0}</td>".format(data['title'].encode('utf-8')))
            fout.write("<td>{0}</td>".format(data['summary'].encode('utf-8')))
            fout.write("</tr>")
            print "<tr>"
        #    print "<td>{0}</td>".format(data['url'].encode('utf-8'))
            print "<td>{0}</td>".format(data['title'].encode('utf-8'))
            print "<td>{0}</td>".format(data['summary'].encode('utf-8'))
            print "</tr>"

        fout.write("</html>")
        fout.write("</body>")
        fout.write("</table>")
        print "</html>"
        print "</body>"
        print "</table>"

        fout.close()
