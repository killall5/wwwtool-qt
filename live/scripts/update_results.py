#!/usr/bin/env python
# -*- coding: utf-8 -*-

from urllib2 import urlopen, quote
from operator import itemgetter
import json
import os
from wwwtool import Command, Game, ResultsView
import datetime
import xml.etree.ElementTree as ET

class SetEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, set):
            return list(obj)
        else:
            return json.JSONEncoder.default(self, obj)


def save_jsoned(data, filename):
    tmp = filename + '.tmp'
    with open(tmp, 'w') as f:
        json.dump(data, f, cls=SetEncoder)
    os.rename(tmp, filename)


def main():
    uri = 'http://192.168.41.10:8080' + quote('/mirror/files/ПерСиб/{year}/ПерСиб-{year}.wg'.format(year=2017))
    updated = datetime.datetime.now().isoformat()

    #uri = 'http://192.168.41.10:8080' + quote('/mirror/files/ПерСиб/2016/Школьники.wg')
    views = [
        ResultsView(tags=[u'Школьники'], title=u'Школьники'),
        ResultsView(tags=[u'Студенты'],  title=u'Студенты'),
    ]
    raw_data = urlopen(uri)
    xml = ET.parse(raw_data).getroot()
    game = Game.parse_xml(xml)
    all_commands = list()
    for x, view in enumerate(views):
        game_view = game.slice_commands(view)
        game_view.calc_results()
        game_view.title = view.title
        serialized = game_view.serialize()
        serialized['updated'] = updated
        all_commands += serialized['res']
        save_jsoned(serialized, '{0}.json'.format(x))

    if all_commands:
        serialized['res'] = all_commands
        save_jsoned(serialized, 'dashboard.json')

    chgklive = game.slice_commands(ResultsView(qmin=45))
    save_jsoned(chgklive.serialize(), 'chgklive.json')

if __name__ == '__main__':
    main()
