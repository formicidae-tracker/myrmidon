from __future__ import annotations

from ._fort_myrmidon import *

import datetime as _dt

_tz = _dt.datetime.now(_dt.timezone.utc).astimezone().tzinfo


def _Time_FromDateTime(dt: _dt.datetime) -> Time:
    """Initializes from a datetime.datetime object.

    Creates a Time from a :class:`datetime.datetime`.
    Args:
        dt (datetime.datetime): a datetime. It will be converted to UTC.
    """
    return Time._FromDateTime(dt.astimezone())


def _Time_ToDateTime(self: Time) -> _dt.datetime:
    """Converts to :class:`datetime.datetime` in local timezone.

    Returns:
        datetime.datetime: a datetime in local timezone.
    """

    return self._ToDateTime().astimezone(_tz)


setattr(Time, "FromDateTime", staticmethod(_Time_FromDateTime))
setattr(Time, "ToDateTime", _Time_ToDateTime)
