#!/usr/bin/env python


class Command():
    def __init__(self, line):
        data = map(lambda x: x.decode('utf-8'), line.split('\t'))
        self.table, self.name, self.location = data[:3]
        self.group = data[4]
        try:
            self.id = data[6]
        except IndexError as e:
            self.id = ''


class Group():
    def __init__(self):
        self.commands = []

    def append(self, command):
        self.commands.append(command)

    def save(self, filename):
        import xml.etree.cElementTree as ET
        WWWTool = ET.Element('WWWTool')
        Commands = ET.SubElement(WWWTool, 'Commands')
        for command in self.commands:
            Command = ET.SubElement(Commands, 'Command')
            ET.SubElement(Command, 'Name').text = command.name
            ET.SubElement(Command, 'Location').text = command.location
            ET.SubElement(Command, 'Id').text = command.id
            ET.SubElement(Command, 'Table').text = command.table

        ET.ElementTree(WWWTool).write(filename)


class Groups():
    def __init__(self):
        self.groups = {}

    def add_command(self, command):
        if command.group not in self.groups:
            self.groups[command.group] = Group()
        self.groups[command.group].append(command)

    def save(self):
        for group_name, group in self.groups.iteritems():
            group.save('%s.wg' % (group_name, ))
                

def main():
    import sys
    groups = Groups()
    for line in sys.stdin:
        command = Command(line.strip())
        groups.add_command(command)

    groups.save()

if __name__ == '__main__':
    main()
