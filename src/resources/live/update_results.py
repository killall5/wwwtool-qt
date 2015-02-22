#!/usr/bin/env python
# -*- coding: utf-8 -*-

from urllib2 import urlopen
from operator import itemgetter
import json
import os

def save_jsoned(data, filename):
    tmp = filename + '.tmp'
    f = open(tmp, 'w')
    json.dump(data, f)
    f.close()
    os.rename(tmp, filename)


def main():
    baseurl = 'http://192.168.41.10:8080/mirror/files/%D0%9F%D0%B5%D1%80%D0%B2%D0%B5%D0%BD%D1%81%D1%82%D0%B2%D0%BE/2015/'
    updatetime = '9999'
    groups = [
        { 'url': baseurl + '01-pupils.wg.json',   'file': '0.json', 'title': u'Школьники', 'place_char': u'Ш' },
        { 'url': baseurl + '02-students.wg.json', 'file': '1.json', 'title': u'Студенты',  'place_char': u'С' },
    ]

    res = []
    fixed = set()
    for group in groups:
        try:
            data = json.load(urlopen(group['url']))
            if 'fixed_question' in data:
                fixed.add(data['fixed_question'])
            uniq_res = {}
            data['res'] = sorted(data['res'], key=lambda cmd: (-cmd[1], -cmd[2], cmd[0]))
            for r in data['res']:
                cmd_res = '%d.%d' % (r[1], r[2])
                if cmd_res in uniq_res:
                    uniq_res[cmd_res] += 1
                else:
                    uniq_res[cmd_res] = 0
                r.append(r[3])
            place_min = place_max = 0
            _a = _r = -1
            for r in data['res']:
                cmd_res = '%d.%d' % (r[1], r[2])
                if r[1] != _a or r[2] != _r:
                    _a = r[1]
                    _r = r[2]
                    place_min = place_max + 1
                    place_max = place_min + uniq_res[cmd_res]
                if place_min == place_max:
                    r[3] = place_min
                else:
                    r[3] = '%d-%d' % (place_min, place_max)
            data['title'] = group['title']
            save_jsoned(data, group['file'])
            for r in data['res']:
                r[3] = group['place_char'] + unicode(r[3])
            res += data['res']
            if updatetime > data['updated']:
                updatetime = data['updated']
        except Exception as e:
            print e
    res = sorted(res, key=lambda cmd: (-cmd[1], -cmd[2], cmd[0]))
    for r in res:
        del r[4]
    if 0 in fixed:
        fixed.remove(0)
    output = {
        'updated': updatetime,
        'fixed_question': list(fixed),
        'res': res,
    }
    save_jsoned(output, 'dashboard.json')

if __name__ == '__main__':
    main()
