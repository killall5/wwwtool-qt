#!/usr/bin/env python

from copy import deepcopy
import unittest
import random


class Command(object):
    def __init__(self, name, table=None, location=None, tags=None):
        self.name = name
        self.table = table
        self.location = location
        self.tags = set() if tags is None else tags
        self.rating = 0
        self.right_answers = set()
        self.place_min = None
        self.place_max = None


class ResultsView(object):
    def __init__(self, qmin=0, qmax=float('inf'), tags=None, title=None):
        self.question_min = qmin
        self.question_max = qmax
        self.tags = set() if tags is None else set(tags)
        self.title = title


class UniqRes(dict):
    def update_uniq(self, key):
        if key in self:
            self[key] += 1
        else:
            self[key] = 0


class Game(object):
    def __init__(self):
        self.commands = []
        self.finished = False
        self.questions = 0
        self.edit_in_progress = set()
        self.title = None

    def slice_commands(self, view):
        assert isinstance(view, ResultsView)
        qmin = max(0, view.question_min)
        qmax = min(self.questions, view.question_max)
        game = deepcopy(self)
        game.commands = []
        game.questions = qmax - qmin
        for command in self.commands:
            if view.tags.issubset(command.tags):
                cmd_copy = deepcopy(command)
                cmd_copy.right_answers = set([x-qmin for x in command.right_answers if qmin<=x and x<qmax])
                game.commands.append(cmd_copy)
        return game

    def calc_results(self):
        ratings = [len(self.commands)+1]*self.questions
        # Calculate questions ratings
        for q in range(self.questions):
            for cmd in self.commands:
                if q in cmd.right_answers:
                    ratings[q] -= 1
        # Calculate commands ratings
        for cmd in self.commands:
            cmd.rating = 0
            for q in cmd.right_answers:
                cmd.rating += ratings[q]
        # Calc uniq results
        uniq_res = UniqRes()
        for cmd in self.commands:
            uniq_res.update_uniq('{0}'.format(len(cmd.right_answers)))
            uniq_res.update_uniq('{0}:{1}'.format(len(cmd.right_answers), cmd.rating))

        # Sort commands
        self.commands = sorted(self.commands, key=lambda x: (-len(x.right_answers), -x.rating, x.name))
        # Fill places
        place_min = place_max = 0
        prev = (-1, -1)
        #import pdb; pdb.set_trace()
        for cmd in self.commands:
            res = (len(cmd.right_answers), cmd.rating)
            inc_place = False
            if place_min < 4:
                if res[0] != prev[0]:
                    inc_place = True
            else:
                if res != prev:
                    inc_place = True
            if inc_place:
                place_min = place_max + 1
                if place_min < 4:
                    place_max = place_min + uniq_res['{0}'.format(res[0])]
                else:
                    place_max = place_min + uniq_res['{0}:{1}'.format(*res)]
            prev = res
            cmd.place_min, cmd.place_max = place_min, place_max

    def serialize(self):
        data = dict(
            qc=self.questions,
            title=self.title or '',
            res=list(),
            fixed_question=self.edit_in_progress,
            finished=self.finished,
            format=dict(
                name=0,
                right_answers_count=1,
                right_answers_array=2,
                rating=3,
                location=4,
                table=5,
                place_min=6,
                place_max=7,
                place=8,
                tags=9,
            )
        )
        for cmd in self.commands:
            data['res'].append([
                cmd.name,
                len(cmd.right_answers),
                list(cmd.right_answers),
                cmd.rating,
                cmd.location or '',
                cmd.table or '',
                cmd.place_min,
                cmd.place_max,
                str(cmd.place_min) if cmd.place_min==cmd.place_max else '{0}-{1}'.format(cmd.place_min, cmd.place_max),
                cmd.tags,
            ])
        return data

    @staticmethod
    def parse_xml(xml):
        game = Game()
        game.questions = int(xml.find('./Options/QuestionCount').text)
        try:
            fixed_question = int(xml.find('./Options/FixedQuestion').text)
            if fixed_question:
                game.edit_in_progress.add(fixed_question)
        except AttributeError, ValueError:
            pass
        try:
            finished = int(xml.find('./Options/Finished').text)
            if finished:
                game.finished = True
        except AttributeError, ValueError:
            pass
        for cmd in xml.findall('./Commands/Command'):
            command = Command(cmd.find('./Name').text, 
                     location=cmd.find('./Location').text,
                     table=cmd.find('./Table').text)
            for tag in cmd.findall('./Tags/Tag'):
                command.tags.add(tag.text)
            for question in cmd.findall('./Question[@value="1"]'):
                command.right_answers.add(int(question.get('number'))-1)
            game.commands.append(command)
        return game


class GameTest(unittest.TestCase):
    def setUp(self):
        self.game = Game()
        self.game.questions = 100
        tags = ['Even', 'Odd']
        for i in range(100):
            cmd = Command('Command {0}'.format(i), tags=set([tags[i%2], 'Tag {0}'.format(i)]))
            cmd.right_answers = set(range(i))
            self.game.commands.append(cmd)

    def test_slice_commands(self):
        view = ResultsView(qmin=50, tags=['Even'])
        sliced_game = self.game.slice_commands(view)
        res = sliced_game.commands
        self.assertEqual(sliced_game.questions, 50)
        self.assertEqual(len(res), 50)
        for x in range(50):
            self.assertEqual(res[x].name, 'Command {0}'.format(x*2))
            self.assertEqual(res[x].right_answers, set(range(x*2-50)))

    def test_slice_command2(self):
        view = ResultsView()
        res = self.game.slice_commands(view).commands
        self.assertEqual(len(res), 100)
        for x in range(100):
            self.assertEqual(res[x].right_answers, set(range(x)))

    def test_calc_results(self):
        game = deepcopy(self.game)
        random.shuffle(game.commands)
        game.calc_results()
        for x in range(100):
            self.assertEqual(game.commands[x].place_min, x+1)
            self.assertEqual(game.commands[x].place_max, x+1)
            self.assertEqual(game.commands[x].rating, sum(range(101-x))-1)

    def test_calc_results2(self):
        game = deepcopy(self.game)
        idx = 0
        for cmd in game.commands:
            cmd.right_answers = set([idx])
            idx += 1
        random.shuffle(game.commands)
        game.calc_results()
        for x in range(100):
            self.assertEqual(game.commands[x].place_min, 1)
            self.assertEqual(game.commands[x].place_max, 100)
            self.assertEqual(game.commands[x].rating, 100)

    def test_calc_results3(self):
        game = deepcopy(self.game)
        idx = 0
        for cmd in game.commands:
            cmd.right_answers = set([idx, 0])
            idx = 1 - idx
        random.shuffle(game.commands)
        game.calc_results()
        for x in range(50):
            self.assertEqual(game.commands[x].place_min, 1)
            self.assertEqual(game.commands[x].place_max, 50)
            self.assertEqual(game.commands[x].rating, 52)
            self.assertEqual(game.commands[x+50].place_min, 51)
            self.assertEqual(game.commands[x+50].place_max, 100)
            self.assertEqual(game.commands[x+50].rating, 1)

    def test_calc_results4(self):
        game = deepcopy(self.game)
        game.commands = game.commands[:6]
        for cmd in game.commands:
            cmd.right_answers = set()
        game.commands[0].right_answers = set([0, 1, 2])
        game.commands[1].right_answers = set([0,       3, 4])
        game.commands[2].right_answers = set([0, 1 ])
        game.commands[3].right_answers = set([   1, 2])
        game.commands[4].right_answers = set([3])
        game.commands[5].right_answers = set([0])
        game.calc_results()
        results = [
        # place_min, place_max, rating
            (1, 2, 14),
            (1, 2, 12),
            (3, 4, 9),
            (3, 4, 7),
            (5, 5, 5),
            (6, 6, 3),
        ]
        for x, res in enumerate(results):
            self.assertEqual(game.commands[x].place_min, res[0])
            self.assertEqual(game.commands[x].place_max, res[1])
            self.assertEqual(game.commands[x].rating, res[2])


if __name__ == '__main__':
    unittest.main()
