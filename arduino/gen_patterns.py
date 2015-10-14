#! /usr/bin/python3

#
# Copyright (c) 2015 HERE Global B.V. and its affiliate(s).
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# 

import sys

def read_character():
  print('  {', end='')

  for i in range(7):
    l = sys.stdin.readline().strip()
    byte = 0
    for c in range(7):
      if l[c] == '.':
        byte = byte << 1
      elif l[c] == '#':
        byte = (byte << 1) | 1
      else:
        raise Exception('Invalid character in bitmap description')

    print('0x%02x%s' % (byte, ', ' if i != 6 else '},\n'), end='')

def main():
  symbol_no = 0

  print('static const byte symbols[][7] = {')

  while True:
    line = sys.stdin.readline().strip()
    if line == '':
      break

    if not line.startswith('#'):
      raise Exception('Invalid symbol format')

    description = line[1:].strip()
    print('  // 0x%02x - %s' % (symbol_no, description))

    read_character()

    symbol_no = symbol_no + 1

  print('};');

if __name__ == '__main__':
  main()
