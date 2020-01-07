__author__ = 'zhr'

from random import choice
import string


def GenPassword(length=16, chars=string.ascii_letters+string.digits):
    return ''.join([choice(chars) for i in range(length)])