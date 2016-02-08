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
    baseurl = 'http://192.168.41.10:8080/mirror/files/%D0%9F%D0%B5%D1%80%D0%B2%D0%B5%D0%BD%D1%81%D1%82%D0%B2%D0%BE/2016/'
    updatetime = '9999'
    groups = [
        { 'url': baseurl + 'Школьники.wg.json', 'file': '0.json', 'title': u'Школьники', 'place_char': u'Ш' },
        { 'url': baseurl + 'Студенты.wg.json',  'file': '1.json', 'title': u'Студенты',  'place_char': u'С' },
    ]

    res = []
    fixed = set()
    format = {}
    for group in groups:
        #try:
            data = json.load(urlopen(group['url']))
            if 'fixed_question' in data:
                fixed.add(data['fixed_question'])
            i_name = data['format']['name']
            i_rac = data['format']['right_answers_count']
            i_rating = data['format']['rating']
            i_raa = data['format']['right_answers_array']
            i_id = data['format']['id']
            i_location = data['format']['location']
            i_table = data['format']['table']
            i_place = 1 + max(data['format'].values())
            data['format']['place'] = i_place
            format = data['format']

            uniq_res = {}
            data['res'] = sorted(data['res'], key=lambda cmd: (-cmd[i_rac], -cmd[i_rating], cmd[i_name]))
            for r in data['res']:
                cmd_res = '%d.%d' % (r[i_rac], r[i_rating])
                if cmd_res in uniq_res:
                    uniq_res[cmd_res] += 1
                else:
                    uniq_res[cmd_res] = 0
                r.append(0)
            place_min = place_max = 0
            _a = _r = -1
            for r in data['res']:
                cmd_res = '%d.%d' % (r[i_rac], r[i_rating])
                if r[i_rac] != _a or r[i_rating] != _r:
                    _a = r[i_rac]
                    _r = r[i_rating]
                    place_min = place_max + 1
                    place_max = place_min + uniq_res[cmd_res]
                if place_min == place_max:
                    r[i_place] = place_min
                else:
                    r[i_place] = '%d-%d' % (place_min, place_max)
            data['title'] = group['title']
            save_jsoned(data, group['file'])
            for r in data['res']:
                r[i_place] = group['place_char'] + unicode(r[i_place])
            res += data['res']
            if updatetime > data['updated']:
                updatetime = data['updated']
        #except Exception as e:
        #    print e
    res = sorted(res, key=lambda cmd: (-cmd[i_rac], -cmd[i_rating], cmd[i_name]))
    if 0 in fixed:
        fixed.remove(0)
    output = {
        'updated': updatetime,
        'fixed_question': list(fixed),
        'format': format,
        'res': res,
    }
    save_jsoned(output, 'dashboard.json')

if __name__ == '__main__':
    main()
