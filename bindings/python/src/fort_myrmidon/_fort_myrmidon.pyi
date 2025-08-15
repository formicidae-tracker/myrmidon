import numpy
from typing import Any, Callable, ClassVar, Iterable, Iterator, overload

PREDECENCE_HIGHER: ZonePriority
PREDECENCE_LOWER: ZonePriority
__version__: str

class Ant:
    """

    Ant are the main object of interest of an :class:`Experiment`. They
    are identified from tags with :class:`Identification`, have a virtual
    shape to perform collision and interaction detection, and holds user
    defined metadata.

    Ant can only be created from an :class:`Experiment` with
    :meth:`Experiment.CreateAnt`.

    Ant are uniquely identified by an :attr:`ID`. By convention we use
    decimal notation with up to two ``0`` prefix to display these AntID, as
    returned by :func:`FormatAntID`.

    Instead of working directly with TagID **fort-myrmidon** uses
    :class:`Identification` to relate a tag value to an Ant. An Ant could
    have different Identifications, allowing us to use different tag ID to
    refer to the same individual. One would use :meth:`IdentifiedAt` to
    obtain the tag ID that identifies an Ant at a given :class:`Time`.

    Each Ant has an associated virtual shape that is used to compute
    instantaneous collision detection ( :func:`Query.CollideFrame` ), or
    timed ant interactions ( :func:`Query.ComputeAntInteraction` ). These
    shapes can be defined manually in **fort-studio** or programmatically
    accessed and modified with :attr:`Capsules`, :meth:`AddCaspule`,
    :meth:`DeleteCapsule` and :meth:`ClearCapsules`.

    Basic visualization of Experiment data can be done through
    **fort-studio**. Ants are displayed according to their
    :attr:`DisplayStatus` and :attr:`DisplayColor`.

    Ant can stores timed user defined metadata. These are modifiable
    using :meth:`SetValue` and :meth:`DeleteValue` and accesible through
    :meth:`GetValue`.

    """

    class DisplayState:
        """Enumerates the possible display state for an Ant

        Members:

          VISIBLE : the Ant is visible

          HIDDEN : the Ant is hidden

          SOLO : the Ant is visible and all other non-solo ant are hidden"""
        __members__: ClassVar[dict] = ...  # read-only
        HIDDEN: ClassVar[Ant.DisplayState] = ...
        SOLO: ClassVar[Ant.DisplayState] = ...
        VISIBLE: ClassVar[Ant.DisplayState] = ...
        __entries: ClassVar[dict] = ...
        def __init__(self, value: int) -> None:
            """__init__(self: fort_myrmidon._fort_myrmidon.Ant.DisplayState, value: int) -> None"""
        def __eq__(self, other: object) -> bool:
            """__eq__(self: object, other: object) -> bool"""
        def __hash__(self) -> int:
            """__hash__(self: object) -> int"""
        def __index__(self) -> int:
            """__index__(self: fort_myrmidon._fort_myrmidon.Ant.DisplayState) -> int"""
        def __int__(self) -> int:
            """__int__(self: fort_myrmidon._fort_myrmidon.Ant.DisplayState) -> int"""
        def __ne__(self, other: object) -> bool:
            """__ne__(self: object, other: object) -> bool"""
        @property
        def name(self) -> str:
            """name(self: object) -> str

            name(self: object) -> str
            """
        @property
        def value(self) -> int:
            """(arg0: fort_myrmidon._fort_myrmidon.Ant.DisplayState) -> int"""
    DisplayColor: tuple[int, int, int]
    DisplayStatus: Ant.DisplayState
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def AddCapsule(self, shapeTypeID: int, capsule: Capsule) -> None:
        """AddCapsule(self: fort_myrmidon._fort_myrmidon.Ant, shapeTypeID: int, capsule: fort_myrmidon._fort_myrmidon.Capsule) -> None


        Adds a Capsule to the Ant virtual shape.

        Args:
            shapeTypeID (int): the AntShapeTypeID associated with the capsule
            capsule (Capsule): the capsule to add

        Raises:
            ValueError: if **shapeTypeID** is not defined in the
                :class:`Experiment`

        """
    def ClearCapsules(self) -> None:
        """ClearCapsules(self: fort_myrmidon._fort_myrmidon.Ant) -> None


        Removes all capsules for this Ant.

        """
    def DeleteCapsule(self, index: int) -> None:
        """DeleteCapsule(self: fort_myrmidon._fort_myrmidon.Ant, index: int) -> None


        Removes one of the shape

        Args:
            index (int): the index to remove in :attr:`Capsules`

        Raises:
            IndexError: if ``index >= len(self.Capsules())``

        """
    def DeleteValue(self, key: str, time: Time) -> None:
        """DeleteValue(self: fort_myrmidon._fort_myrmidon.Ant, key: str, time: fort_myrmidon._fort_myrmidon.Time) -> None


        Clears a user defined timed metadata.

        Args:
            key (str): the key to clear
            time (Time): the time to clear a value for key

        Raises:
            IndexError: if **key** was not previously set for **time** with
                :meth:`SetValue`.

        """
    def GetValue(self, key: str, time: Time) -> bool | int | float | str | Time:
        """GetValue(self: fort_myrmidon._fort_myrmidon.Ant, key: str, time: fort_myrmidon._fort_myrmidon.Time) -> Union[bool, int, float, str, fort_myrmidon._fort_myrmidon.Time]


        Gets user defined timed metadata.

        Args:
            key (str): the key to query
            time (Time): the time, possibly infinite to query for

        Returns:
            bool, int, float, str or Time: the value for **key** at **time**,
            if defined, or the Experiment's default value.

        Raises:
            IndexError: if **key** is not defined in :class:`Experiment`

        """
    def GetValues(self, key: str) -> list[tuple[Time, bool | int | float | str | Time]]:
        """GetValues(self: fort_myrmidon._fort_myrmidon.Ant, key: str) -> list[tuple[fort_myrmidon._fort_myrmidon.Time, Union[bool, int, float, str, fort_myrmidon._fort_myrmidon.Time]]]


        Gets metadata key changes over time.

        Args:
            key (str): the key to list changes

        Raises:
            IndexError: if **key** is not defined in :class:`Experiment`

        """
    def IdentifiedAt(self, time: Time) -> int:
        """IdentifiedAt(self: fort_myrmidon._fort_myrmidon.Ant, time: fort_myrmidon._fort_myrmidon.Time) -> int


        Gets the TagID identifiying this Ant at a given time

        Args:
            time (Time): the time we want an identification for

        Returns:
            int: the TagID that identifies this Ant at time.

        Raises:
            Error: if no tag identifies this Ant at **time**.

        """
    def SetValue(self, key: str, value: bool | int | float | str | Time, time: Time) -> None:
        """SetValue(self: fort_myrmidon._fort_myrmidon.Ant, key: str, value: Union[bool, int, float, str, fort_myrmidon._fort_myrmidon.Time], time: fort_myrmidon._fort_myrmidon.Time) -> None


        Sets a user defined timed metadata

        Args:
            key (str): the key to defined
            value (bool, int, float, str or Time): the wanted
                value.
            time (Time): the first Time where **key** will be set to
                **value**. It can be :meth:`Time.SinceEver`

        Raises:
            IndexError: if **key** is not defined in the :class:`Experiment`
            ValueError: if **time** is :meth:`Time.Forever`
            RuntimeError: if **value** is not of the right type for **key**

        """
    @property
    def Capsules(self) -> list[tuple[int, Capsule]]:
        """List[Tuple[int,Capsule]]: a list of capsules and their type
        (arg0: fort_myrmidon._fort_myrmidon.Ant) -> list[tuple[int, fort_myrmidon._fort_myrmidon.Capsule]]
        """
    @property
    def ID(self) -> int:
        """int: the AntID for this Ant
        (arg0: fort_myrmidon._fort_myrmidon.Ant) -> int
        """
    @property
    def Identifications(self) -> list[Identification]:
        """List[Identification]: all Identification that target this Ant, ordered by validity time.
        (arg0: fort_myrmidon._fort_myrmidon.Ant) -> list[fort_myrmidon._fort_myrmidon.Identification]
        """

class AntInteraction:
    """Represent an interaction between two Ant"""
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def HasInteractionType(self, arg0: int, arg1: int) -> bool:
        """HasInteractionType(self: fort_myrmidon._fort_myrmidon.AntInteraction, arg0: int, arg1: int) -> bool

        Tests if interaction contains a given interaction type.

        Args:
            type1(int): The first type to test again.
            type2(int): The second type to test again.

        Returns:
            bool: True if the interaction type (type1,type2) is present. Note that order matter.

        """
    @property
    def End(self) -> Time:
        """Time: the end Time of the interaction.
        (self: fort_myrmidon._fort_myrmidon.AntInteraction) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def IDs(self) -> tuple[int, int]:
        """Tuple[int,int]: the AntIDs of the two Ant interaction
        (self: fort_myrmidon._fort_myrmidon.AntInteraction) -> tuple[int, int]
        """
    @property
    def Space(self) -> int:
        """int: the SpaceID of the Space the interaction takes place.
        (self: fort_myrmidon._fort_myrmidon.AntInteraction) -> int
        """
    @property
    def Start(self) -> Time:
        """Time: the start Time of the interaction.
        (self: fort_myrmidon._fort_myrmidon.AntInteraction) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def Trajectories(self) -> tuple[AntTrajectorySegment, AntTrajectorySegment] | tuple[AntTrajectorySummary, AntTrajectorySummary]:
        """Union[Tuple[AntTrajectorySegment,AntTrajectorySegment],Tuple[AntTrajectorySummary,AntTrajectorySummary]]: The two section of trajectory for the two Ant during this interaction. Either the segments or their summaries.
        (self: fort_myrmidon._fort_myrmidon.AntInteraction) -> Union[tuple[fort_myrmidon._fort_myrmidon.AntTrajectorySegment, fort_myrmidon._fort_myrmidon.AntTrajectorySegment], tuple[fort_myrmidon._fort_myrmidon.AntTrajectorySummary, fort_myrmidon._fort_myrmidon.AntTrajectorySummary]]
        """
    @property
    def Types(self) -> numpy.ndarray[numpy.uint32[m, 2]]:
        """numpy.ndarray: The AntShapeTypeID that were in contact during the interaction. Any body part interacting at least once will add a row in this array. The first column refers to the first Ant, and the second column to the other Ant.
        (arg0: fort_myrmidon._fort_myrmidon.AntInteraction) -> numpy.ndarray[numpy.uint32[m, 2]]
        """

class AntTrajectory:
    """An Ant trajectory represents a continuous spatial trajectory of an Ant"""
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def End(self) -> Time:
        """End(self: fort_myrmidon._fort_myrmidon.AntTrajectory) -> fort_myrmidon._fort_myrmidon.Time


        Computes the End time of the AntTrajectory.

        Returns:
            Time: the last Time found in this trajectory. It does not contains Duration.

        """
    @property
    def Ant(self) -> int:
        """int: the AntID of the Ant
        (self: fort_myrmidon._fort_myrmidon.AntTrajectory) -> int
        """
    @property
    def Duration_s(self) -> float:
        """float: the duration (including last frame duration) of this trajectory.
        (self: fort_myrmidon._fort_myrmidon.AntTrajectory) -> float
        """
    @property
    def Positions(self) -> numpy.ndarray[numpy.float64[m, 5]]:
        """numpy.ndarray: a N row array of position. Columns are (t,x,y,angle,zone), where t is the offset from Start in seconds.
        (arg0: fort_myrmidon._fort_myrmidon.AntTrajectory) -> numpy.ndarray[numpy.float64[m, 5]]
        """
    @property
    def Space(self) -> int:
        """int: the SpaceID where the trajectory takes place.
        (self: fort_myrmidon._fort_myrmidon.AntTrajectory) -> int
        """
    @property
    def Start(self) -> Time:
        """Time: the starting time of this trajectory.
        (self: fort_myrmidon._fort_myrmidon.AntTrajectory) -> fort_myrmidon._fort_myrmidon.Time
        """

class AntTrajectorySegment:
    """
    Represents a section  of an :class:`AntTrajectory`.
    """
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def EndTime(self) -> Time:
        """EndTime(self: fort_myrmidon._fort_myrmidon.AntTrajectorySegment) -> fort_myrmidon._fort_myrmidon.Time


        Computes the ending Time of the AntTrajectorySegment

        Returns:
            Time: the ending Time of the AntTrajectorySegment.

        """
    def StartTime(self) -> Time:
        """StartTime(self: fort_myrmidon._fort_myrmidon.AntTrajectorySegment) -> fort_myrmidon._fort_myrmidon.Time


        Computes the starting Time of the AntTrajectorySegment

        Returns:
            Time: the starting Time of the AntTrajectorySegment.

        """
    @property
    def Begin(self) -> int:
        """int: the first index in Trajectory this segment refers to.
        (self: fort_myrmidon._fort_myrmidon.AntTrajectorySegment) -> int
        """
    @property
    def End(self) -> int:
        """int: the last index+1 in Trajectory this segment refers to.
        (self: fort_myrmidon._fort_myrmidon.AntTrajectorySegment) -> int
        """
    @property
    def Trajectory(self) -> AntTrajectory:
        """Trajectory: the AntTrajectory it refers to.
        (self: fort_myrmidon._fort_myrmidon.AntTrajectorySegment) -> fort_myrmidon._fort_myrmidon.AntTrajectory
        """

class AntTrajectorySummary:
    """
    Represents a summary  of an :class:`AntTrajectory` section.
    """
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @property
    def Mean(self) -> numpy.ndarray[numpy.float64[3, 1]]:
        """Trajectory: the AntTrajectory it refers to.
        (self: fort_myrmidon._fort_myrmidon.AntTrajectorySummary) -> numpy.ndarray[numpy.float64[3, 1]]
        """
    @property
    def Zones(self) -> set[int]:
        """List[int]: all the ZoneID the trajectory crossed.
        (self: fort_myrmidon._fort_myrmidon.AntTrajectorySummary) -> set[int]
        """

class Capsule(Shape):
    """
    Represents a capsule

    A capsule is the region inside and between two given circles.
    """
    C1: numpy.ndarray[numpy.float64[2, 1]]
    C2: numpy.ndarray[numpy.float64[2, 1]]
    R1: float
    R2: float
    def __init__(self, C1: numpy.ndarray[numpy.float64[2, 1]] = ..., C2: numpy.ndarray[numpy.float64[2, 1]] = ..., R1: float = ..., R2: float = ...) -> None:
        """__init__(self: fort_myrmidon._fort_myrmidon.Capsule, C1: numpy.ndarray[numpy.float64[2, 1]] = array([0., 0.]), C2: numpy.ndarray[numpy.float64[2, 1]] = array([1., 1.]), R1: float = 1.0, R2: float = 1.0) -> None"""

class Circle(Shape):
    """
    Represents a circle
    """
    Center: numpy.ndarray[numpy.float64[2, 1]]
    Radius: float
    def __init__(self, Center: numpy.ndarray[numpy.float64[2, 1]] = ..., Radius: float = ...) -> None:
        """__init__(self: fort_myrmidon._fort_myrmidon.Circle, Center: numpy.ndarray[numpy.float64[2, 1]] = array([0., 0.]), Radius: float = 1.0) -> None"""

class Collision:
    """A Collision describe an instantaneous contact between two ants"""
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @property
    def IDs(self) -> tuple[int, int]:
        """Tuple[int,int]: the AntIDs of the two ants. IDs are always ordered from smaller to higher.
        (self: fort_myrmidon._fort_myrmidon.Collision) -> tuple[int, int]
        """
    @property
    def Types(self) -> numpy.ndarray[numpy.uint32[m, 2]]:
        """numpy.ndarray: an N row array describing the colliding AntShapeTypeID. First column refers to shape type of the first Ant, which are colliding with a part of the second Ant in the second column.
        (arg0: fort_myrmidon._fort_myrmidon.Collision) -> numpy.ndarray[numpy.uint32[m, 2]]
        """
    @property
    def Zone(self) -> int:
        """int: the ZoneID where the collision happens
        (self: fort_myrmidon._fort_myrmidon.Collision) -> int
        """

class CollisionFrame:
    """A CollisionFrame regroups all Collision that happen in a video frame"""
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @property
    def Collisions(self) -> list[Collision]:
        """List[Collision]: the list of Collision in the frame
        (self: fort_myrmidon._fort_myrmidon.CollisionFrame) -> list[fort_myrmidon._fort_myrmidon.Collision]
        """
    @property
    def FrameTime(self) -> Time:
        """Time: the Time the video frame was acquired
        (self: fort_myrmidon._fort_myrmidon.CollisionFrame) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def Space(self) -> int:
        """int: the Space the video frame belongs to
        (self: fort_myrmidon._fort_myrmidon.CollisionFrame) -> int
        """

class ComputedMeasurement:
    """
    A manual **fort-studio** measurement and its estimated value in
    millimeters.
    """
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @property
    def LengthMM(self) -> float:
        """float: its length in millimeters.
        (self: fort_myrmidon._fort_myrmidon.ComputedMeasurement) -> float
        """
    @property
    def LengthPixel(self) -> float:
        """float: its length in pixel.
        (self: fort_myrmidon._fort_myrmidon.ComputedMeasurement) -> float
        """
    @property
    def Time(self) -> Time:
        """Time: the Time of the close-up this measurement.
        (self: fort_myrmidon._fort_myrmidon.ComputedMeasurement) -> fort_myrmidon._fort_myrmidon.Time
        """

class Duration:
    """
    Represents an amount of nanoseconds as a signed 64-bit integer

    Note:
        The notion of 64-bit integer does not exist natively in python,
        but since the mapped c++ object is, it will works like a 64-bit
        integer.

    Attributes:
        Hour (Duration): the value for an hour
        Minute (Duration): the value for a minute
        Second (Duration): the value for a second
        Millisecond (Duration): the value for a millisecond
        Microsecond (Duration): the value for a microsecond
    """
    Hour: ClassVar[Duration] = ...  # read-only
    Microsecond: ClassVar[Duration] = ...  # read-only
    Millisecond: ClassVar[Duration] = ...  # read-only
    Minute: ClassVar[Duration] = ...  # read-only
    Second: ClassVar[Duration] = ...  # read-only
    @overload
    def __init__(self, ns: int) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Duration, ns: int) -> None


        Initialize a Duration from an amount of nanoseconds

        Args:
            ns (int): the number of nanosecond to represent


        2. __init__(self: fort_myrmidon._fort_myrmidon.Duration, ns: float) -> None


        Initialize a Duration from an amount of nanoseconds

        Args:
            ns (float): the number of nanosecond to represent


        3. __init__(self: fort_myrmidon._fort_myrmidon.Duration) -> None

        Initialize a zero second Duration.
        """
    @overload
    def __init__(self, ns: float) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Duration, ns: int) -> None


        Initialize a Duration from an amount of nanoseconds

        Args:
            ns (int): the number of nanosecond to represent


        2. __init__(self: fort_myrmidon._fort_myrmidon.Duration, ns: float) -> None


        Initialize a Duration from an amount of nanoseconds

        Args:
            ns (float): the number of nanosecond to represent


        3. __init__(self: fort_myrmidon._fort_myrmidon.Duration) -> None

        Initialize a zero second Duration.
        """
    @overload
    def __init__(self) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Duration, ns: int) -> None


        Initialize a Duration from an amount of nanoseconds

        Args:
            ns (int): the number of nanosecond to represent


        2. __init__(self: fort_myrmidon._fort_myrmidon.Duration, ns: float) -> None


        Initialize a Duration from an amount of nanoseconds

        Args:
            ns (float): the number of nanosecond to represent


        3. __init__(self: fort_myrmidon._fort_myrmidon.Duration) -> None

        Initialize a zero second Duration.
        """
    def Hours(self) -> float:
        """Hours(self: fort_myrmidon._fort_myrmidon.Duration) -> float


            this Duration in hours.

            Returns:
                float: the duration as an amount of hours

        """
    def Microseconds(self) -> float:
        """Microseconds(self: fort_myrmidon._fort_myrmidon.Duration) -> float


        This Duration in microseconds.

        Returns:
            float: the duration as an amount of microseconds

        """
    def Milliseconds(self) -> float:
        """Milliseconds(self: fort_myrmidon._fort_myrmidon.Duration) -> float


        This Duration in milliseconds.

        Returns:
            float: the duration as an amount of milliseconds

        """
    def Minutes(self) -> float:
        """Minutes(self: fort_myrmidon._fort_myrmidon.Duration) -> float


        This Duration in minutes.

        Returns:
            float: the duration as an amount of minutes

        """
    def Nanoseconds(self) -> int:
        """Nanoseconds(self: fort_myrmidon._fort_myrmidon.Duration) -> int


        This Duration in nanoseconds.

        Returns:
            int: the duration as an amount of nanoseconds

        """
    @staticmethod
    def Parse(d: str) -> Duration:
        """Parse(d: str) -> fort_myrmidon._fort_myrmidon.Duration


        Parses a string to a Duration.

        Args:
            d (str): a string in the format `[amount][unit]` as a
                duration. `[amout]` is a value that may contain a decimal
                point, and `[units]` could be any of
                'h','m','s','ms','us','ns'. The pattern can be repeated
                (i.e. '4m32s' is valid).

        Returns:
            Duration: the parsed value

        Raises:
            RuntimeError: when the parsed amount will not hold in a 64-bit
                signed integer

        """
    def Seconds(self) -> float:
        """Seconds(self: fort_myrmidon._fort_myrmidon.Duration) -> float


        This Duration in seconds.

        Returns:
            float: the duration as an amount of seconds

        """
    @overload
    def __add__(self, arg0: Duration) -> Duration:
        """__add__(*args, **kwargs)
        Overloaded function.

        1. __add__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Duration

        2. __add__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: int) -> fort_myrmidon._fort_myrmidon.Duration
        """
    @overload
    def __add__(self, arg0: int) -> Duration:
        """__add__(*args, **kwargs)
        Overloaded function.

        1. __add__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Duration

        2. __add__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: int) -> fort_myrmidon._fort_myrmidon.Duration
        """
    def __eq__(self, arg0: Duration) -> bool:
        """__eq__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> bool"""
    def __ge__(self, arg0: Duration) -> bool:
        """__ge__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> bool"""
    def __gt__(self, arg0: Duration) -> bool:
        """__gt__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> bool"""
    def __le__(self, arg0: Duration) -> bool:
        """__le__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> bool"""
    def __lt__(self, arg0: Duration) -> bool:
        """__lt__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> bool"""
    @overload
    def __mul__(self, arg0: Duration) -> Duration:
        """__mul__(*args, **kwargs)
        Overloaded function.

        1. __mul__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Duration

        2. __mul__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: int) -> fort_myrmidon._fort_myrmidon.Duration
        """
    @overload
    def __mul__(self, arg0: int) -> Duration:
        """__mul__(*args, **kwargs)
        Overloaded function.

        1. __mul__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Duration

        2. __mul__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: int) -> fort_myrmidon._fort_myrmidon.Duration
        """
    def __radd__(self, arg0: int) -> Duration:
        """__radd__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: int) -> fort_myrmidon._fort_myrmidon.Duration"""
    def __rmul__(self, arg0: int) -> Duration:
        """__rmul__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: int) -> fort_myrmidon._fort_myrmidon.Duration"""
    def __rsub__(self, arg0: int) -> Duration:
        """__rsub__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: int) -> fort_myrmidon._fort_myrmidon.Duration"""
    @overload
    def __sub__(self, arg0: Duration) -> Duration:
        """__sub__(*args, **kwargs)
        Overloaded function.

        1. __sub__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Duration

        2. __sub__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: int) -> fort_myrmidon._fort_myrmidon.Duration
        """
    @overload
    def __sub__(self, arg0: int) -> Duration:
        """__sub__(*args, **kwargs)
        Overloaded function.

        1. __sub__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Duration

        2. __sub__(self: fort_myrmidon._fort_myrmidon.Duration, arg0: int) -> fort_myrmidon._fort_myrmidon.Duration
        """

class Experiment:
    """An Experiment olds a collection of :class:`Ant`,
    :class:`Identification`, :class:`Space` and :class:`Zone` and give
    access to the identified tracking data instantaneous collision and
    interaction detection through :class:`Query`.

    Experiment are save to the filesystem in `.myrmidon` files. One can
    use :meth:`Open`, :meth:`OpenDataLess`, and :meth:`Save` to interact
    with these files.

    One can use :meth:`AddTrackingDataDirectory` to link an Experiment
    with some tracking data, organized by **fort-leto** in a tracking data
    directory. This data must be assigned to a :class:`Space` (previously
    created with :meth:`CreateSpace`).  Experiment saves relative links to
    these tracking data directory. These paths are relative, so one can
    rename a `.myrmidon` file on the filesystem with :meth:`Save`, but it
    must remains in the same directory.

    In **fort-myrmidon**, tags are not used directly. Instead user are
    required to make :class:`Ant` object (through :meth:`CreateAnt`) and
    use :class:`Identification` (through :meth:`AddIdentification`) to
    relate a tag value to an Ant. To perform collision and interaction
    detection, users can create for each Ant a virtual shape, made of a
    collection of :class:`Capsule`. Each Capsule is assigned an
    AntShapeTypeID (an integer starting from 1) which must be previously
    defined using :meth:`CreateAntShapeType`. There is no predefined
    AntShapeTypeID.

    **fort-studio** allows to make measurement on close-up of each
    :class:`Ant`. These measurement must be assigned to a type, created
    with :meth:`CreateMeasurementType`. There is a predefined, undeletable
    MeasurementTypeID: :attr:`HEAD_TAIL_MEASUREMENT_TYPE`. It is used to
    automatically determine :attr:`Identification.AntPosition` and
    :attr:`Identification.AntAngle` from **fort-studio** measurement.

    Each :class:`Ant` can also holds a dictionnary of key/value pairs. The
    key name, type and initial value for each Ant must be defined with
    :meth:`SetMetaDataKey`. Through :meth:`Ant.SetValue`, individual,
    timed value can be assigned to each Ant. There are no predefined keys.

    An Experiment is also usuable without linking to any tracking
    data. :meth:`OpenDataLess` can be used to open an existing Experiment,
    previously linked with acquired data, but without requiring the data
    to be present on the filesystem. Any :class:`Query` on such Experiment
    object will report no data, but a :class:`TrackingSolver` (acquired
    with :meth:`CompileTrackingSolver`) could be used, to perform, for
    example identifications and collision detection on a live tracking
    datastream of **fort-leto**. Also tracking and user-defined meta-data
    can be manipulated without the need of the often very large tracking
    data directory to be present on the filesystem.
    """
    HEAD_TAIL_MEASUREMENT_TYPE_ID: ClassVar[int] = ...  # read-only
    Author: str
    Comment: str
    DefaultTagSize: float
    Name: str
    def __init__(self, filepath: str) -> None:
        """__init__(self: fort_myrmidon._fort_myrmidon.Experiment, filepath: str) -> None

        Initialize an experiment from a specified filesystem
        location. This location will be used to determine relative path to
        the tracking data.

        Args:
            filepath (str): the wanted filesystem path to the experiment.
        Returns:
            Experiment: a new empty Experiment associated with **filepath**

        """
    def AddIdentification(self, antID: int, tagID: int, start: Time = ..., end: Time = ...) -> Identification:
        """AddIdentification(self: fort_myrmidon._fort_myrmidon.Experiment, antID: int, tagID: int, start: fort_myrmidon._fort_myrmidon.Time = -∞, end: fort_myrmidon._fort_myrmidon.Time = +∞) -> fort_myrmidon._fort_myrmidon.Identification

        Adds an Identification to the Experiment.

        Args:
            antID (int): the ant to target
            tagID (int): the TagID to use
            start (Time): the first valid Time for the :class:`Identification`
            end (Time): the first invalid Time for the :class:`Identification`

        Returns:
            Identification: the newly created Identification

        Raises:
            IndexError: if **antID** is not valid for the Experiment
            OverlappingIdentification: if the resulting Identification would
                overlap in time with another one, either for a given **antID**
                or **tagID**.

        """
    def AddTrackingDataDirectory(self, spaceID: int, filepath: str, fixCorruptedData: bool = ...) -> str:
        """AddTrackingDataDirectory(self: fort_myrmidon._fort_myrmidon.Experiment, spaceID: int, filepath: str, fixCorruptedData: bool = False) -> str

        Adds a tracking data directory to the Experiment.

        Args:
            spaceID (int): the space to add the tracking data directory
                to.
            filepath (str): the filepath to the tracking data directory.
            fixCorruptedData (bool): In the event that some tracking data is
                corrupted, if False a FixableError will be raised. Otherwise
                an attempt to recover as much data as possible eill be made,
                but it may potentially remove a large chunk of data.

        Returns:
            str: the relative path from self.AbsoluteFilePath to **filepath**,
                that will be the URI to identify the tracking data directory.

        Raises:
            IndexError: if **spaceID** is not valid for this Experiment
            RuntimeError: if **filepath** is not a valid tracking data
                directory.
            FixableError: if **fixCorruptedData** is False and any data
                corruption is found.
            RuntimeError: if the data will overlap in time with another
                directory in the same space
            RuntimeError: if the data is used by another space
            ValueError: if the tag family does not match with other directory
                in the experiment

        """
    def CompileTrackingSolver(self, collisionsIgnoreZones: bool = ...) -> TrackingSolver:
        """CompileTrackingSolver(self: fort_myrmidon._fort_myrmidon.Experiment, collisionsIgnoreZones: bool = False) -> fort_myrmidon._fort_myrmidon.TrackingSolver

        Compiles a :class:`TrackingSolver` that can be used to identify and
        collide ant from raw data.

        Returns:
            TrackingSolver: the compiled tracking solver.

        """
    def CreateAnt(self) -> Ant:
        """CreateAnt(self: fort_myrmidon._fort_myrmidon.Experiment) -> fort_myrmidon._fort_myrmidon.Ant


        Creates a new Ant in the Experiment.

        Returns:
            Ant: the newly created Ant

        """
    def CreateAntShapeType(self, name: str) -> int:
        """CreateAntShapeType(self: fort_myrmidon._fort_myrmidon.Experiment, name: str) -> int

        Creates a new Ant shape type.

        Args:
            name (str): the name of the ant shape type

        Returns:
            int: the AntShapeTypeID for the new type

        """
    def CreateMeasurementType(self, name: str) -> int:
        """CreateMeasurementType(self: fort_myrmidon._fort_myrmidon.Experiment, name: str) -> int

        Creates a new measurement type.

        Args:
            name (str): the name of the measurement type

        Returns:
            int: the MeasurementTypeID for the new type

        """
    def CreateSpace(self, name: str) -> Space:
        """CreateSpace(self: fort_myrmidon._fort_myrmidon.Experiment, name: str) -> fort_myrmidon._fort_myrmidon.Space

        Creates a new Space in this Experiment.

        Args:
            name (str): the name for the new space

        Returns:
            Space: the newly created Space

        """
    def DeleteAnt(self, antID: int) -> None:
        """DeleteAnt(self: fort_myrmidon._fort_myrmidon.Experiment, antID: int) -> None

        Deletes an Ant from the Experiment.

        Args:
            antID (int): the AntID of the Ant to remove
        Raises:
            IndexError: if **antID** is invalid for the experiment
            RuntimeError: if the ant still have Identification targetting
                her

        """
    def DeleteAntShapeType(self, antShapeTypeID: int) -> None:
        """DeleteAntShapeType(self: fort_myrmidon._fort_myrmidon.Experiment, antShapeTypeID: int) -> None

        Deletes an Ant shape type.

        Args:
            antShapeTypeID (int): the type to delete

        Raises:
            IndexError: if **antShapeTypeID** is not valid for Experiment

        """
    def DeleteIdentification(self, identification: Identification) -> None:
        """DeleteIdentification(self: fort_myrmidon._fort_myrmidon.Experiment, identification: fort_myrmidon._fort_myrmidon.Identification) -> None

        Deletes an Identification from this Experiment.

        Args:
            identification (Identification): the Identification to remove

        Raises:
            ValueError: if **identification** is not a valid Identification
                object from this Experiment.

        """
    def DeleteMeasurementType(self, measurementTypeID: int) -> None:
        """DeleteMeasurementType(self: fort_myrmidon._fort_myrmidon.Experiment, measurementTypeID: int) -> None

        Deletes a measurement type.

        Args:
            measurementTypeID (int): the measurement type to delete

        Raises:
            IndexError: if **measurementTypeID** is not valid for Experiment
            ValueError: if measurementTypeID is
                :attr:`HEAD_TAIL_MEASUREMENT_TYPE_ID`

        """
    def DeleteMetaDataKey(self, key: str) -> None:
        """DeleteMetaDataKey(self: fort_myrmidon._fort_myrmidon.Experiment, key: str) -> None

        Deletes a meta data key.

        Args:
            key (str): the key to delete

        Raises:
            IndexError: if **key** is not valid for this Experiment
            RuntimeError: if any :class:`Ant` contains timed data for **key**

        """
    def DeleteSpace(self, spaceID: int) -> None:
        """DeleteSpace(self: fort_myrmidon._fort_myrmidon.Experiment, spaceID: int) -> None

        Deletes a Space from this Experiment.

        Args:
            spaceID (str): the spaceID of this space

        Raises:
            IndexError: if **spaceID** is not a valid for this Experiment.

        """
    def EnsureAllDataIsLoaded(self, fixCorruptedData: bool = ...) -> None:
        """EnsureAllDataIsLoaded(self: fort_myrmidon._fort_myrmidon.Experiment, fixCorruptedData: bool = False) -> None

        Ensures all non-tracking data is loaded.

        Ensures that all non-tracking data, like statistics, fullframes and close-up are
        available.

        Args:
            fixCorruptedData (bool): if True, will silently fix any data
                corruption. This could lead to the loss of large chunck of
                tracking data. Otherwise a RuntimeError is raised summarizing
                all data corruption found.
            displayToStderr (bool): if True, any errors will be logged to stderr.

        Raises:
            RuntimeError: if fixCorruptedData is False and some data
                corruption is found.

        """
    def FreeIdentificationRangeAt(self, tagID: int, time: Time) -> tuple[Time, Time]:
        """FreeIdentificationRangeAt(self: fort_myrmidon._fort_myrmidon.Experiment, tagID: int, time: fort_myrmidon._fort_myrmidon.Time) -> tuple[fort_myrmidon._fort_myrmidon.Time, fort_myrmidon._fort_myrmidon.Time]

        Returns an available time range for a tag.

        Args:
            tagID (int): the tagID to query for
            time (Time): a time that must be contained in the result.

        Returns:
            Tuple[:class:`Time`, :class:`Time`]: A time range (can be
            (:meth:`Time.SinceEver` , :meth:`Time.Forever` )) containing
            **time** where **tagID** is not used by any Identification.

        Raises:
            RuntimeError: if **tagID** already identifies an Ant at **time**

        """
    def IdentificationsAt(self, *args, **kwargs):
        """IdentificationsAt(self: fort_myrmidon._fort_myrmidon.Experiment, time: fort_myrmidon._fort_myrmidon.Time, *, removeUnidentifiedAnt: bool = True) -> dict[int, int]

        Gets AntID <-> TagID correspondances at a given Time.

        Args:

            time (Time): the wanted Time for the correspondance matching.

            removeUnidentifiedAnt (boolean): if true, :class:`Ant` without an
                :class:`Identification` at **time** will not be part of the
                result. Otherwise the associated tagID value will be 2^32-1.

         Returns:
            Dict[int,int]: TagID indexed by their associated AntID.
 
        """
    @staticmethod
    def Open(filepath: str, fixCorruptedData: bool = ...) -> Experiment:
        """Open(filepath: str, fixCorruptedData: bool = False) -> fort_myrmidon._fort_myrmidon.Experiment

        Opens an existing Experiment on the filesystem

        Args:
            filepath (str): the filepath to open.

        Returns:
            Experiment: the Experiment located at filepath

        Raises:
            RuntimeError: if **filepath** does not contains a valid Experiment
                or associated tracking data is not accessible.

        """
    @staticmethod
    def OpenDataLess(filepath: str) -> Experiment:
        """OpenDataLess(filepath: str) -> fort_myrmidon._fort_myrmidon.Experiment

        Opens an existing Experiment on the filesystem in dataless mode.

        In dataless mode, no associated tracking data will be opened, but
        a :class:`TrackingSolver` can be used to identify Ants in live tracking
        situation.

        Args:
            filepath (str): the filepath to open.

        Returns:
            Experiment: the Experiment located at filepath

        Raises:
            RuntimeError: if **filepath** does not contains a valid Experiment

        """
    def RemoveTrackingDataDirectory(self, URI: str) -> None:
        """RemoveTrackingDataDirectory(self: fort_myrmidon._fort_myrmidon.Experiment, URI: str) -> None

        Removes a tracking data directory from the Experiment.

        Args:
            URI (str): the URI that identifies the tracking data directory

        Raises:
            IndexError: if **URI** does not identifies a tracking data
                directory in this experiment.

        """
    def RenameMetaDataKey(self, oldKey: str, newKey: str) -> None:
        """RenameMetaDataKey(self: fort_myrmidon._fort_myrmidon.Experiment, oldKey: str, newKey: str) -> None

        Renames a meta data key.

        Args:
            oldKey (str): the old name of the key
            newKey (str): the new name for the key

        Raises:
            IndexError: if **oldKey** is invalid for the Experiment
            ValueError: if **newKey** is in use for the Experiment

        """
    def Save(self, filepath: str) -> None:
        """Save(self: fort_myrmidon._fort_myrmidon.Experiment, filepath: str) -> None

        Saves the experiment on the filesystem.

        Args:
            filepath: the filepath to save the experiment to.

        Raises:
            ValueError: if **filepath** would change the directory of the
                Experiment on the filesystem.

        """
    def SetAntShapeTypeName(self, antShapeTypeID: int, name: str) -> None:
        """SetAntShapeTypeName(self: fort_myrmidon._fort_myrmidon.Experiment, antShapeTypeID: int, name: str) -> None

        Sets the name for an Ant shape type.

        Args:
            antShapeTypeID (int): the type to modify
            name (str): the wanted name

        Raises:
            IndexError: if **antShapeTypeID** is invalid for this Experiment

        """
    def SetMeasurementTypeName(self, measurementTypeID: int, name: str) -> None:
        """SetMeasurementTypeName(self: fort_myrmidon._fort_myrmidon.Experiment, measurementTypeID: int, name: str) -> None

        Sets the name for a measurement type.

        Args:
            measurementTypeID (int): the type to modify
            name (str): the wanted name

        Raises:
            IndexError: if **measurementTypeID** is invalid for this
                Experiment

        """
    def SetMetaDataKey(self, key: str, defaultValue: bool | int | float | str | Time) -> None:
        """SetMetaDataKey(self: fort_myrmidon._fort_myrmidon.Experiment, key: str, defaultValue: Union[bool, int, float, str, fort_myrmidon._fort_myrmidon.Time]) -> None

        Adds or modifies a meta data key.

        Args:
            key (str): the key to modify
            defaultValue (object): the default value for the key. It will sets
                its type. Must be a boolean, an int, a float, a str or a
                :class:`Time`

        Raises:
            RuntimeError: if the following conditions are met: a) **key** is
                already registered, b) **defaultValue** would change the type
                of **key** and c) at least one :class:`Ant` has a value
                registered for **key**

        """
    @property
    def AbsoluteFilePath(self) -> str:
        """str: the absolute filepath of the Experiment
        (arg0: fort_myrmidon._fort_myrmidon.Experiment) -> str
        """
    @property
    def AntShapeTypeNames(self) -> dict[int, str]:
        """Dict[int,str]: the ant shape type name by their AntShapeTypeID
        (arg0: fort_myrmidon._fort_myrmidon.Experiment) -> dict[int, str]
        """
    @property
    def Ants(self) -> dict[int, Ant]:
        """Dict[int,Ant]: the Ant indexed by their AntID.
        (arg0: fort_myrmidon._fort_myrmidon.Experiment) -> dict[int, fort_myrmidon._fort_myrmidon.Ant]
        """
    @property
    def Family(self) -> TagFamily:
        """TagFamily: the TagFamily used in the Experiment
        (arg0: fort_myrmidon._fort_myrmidon.Experiment) -> fort_myrmidon._fort_myrmidon.TagFamily
        """
    @property
    def MeasurementTypeNames(self) -> dict[int, str]:
        """Dict[int,str]: the measurement type name by their MeasurementTypeID
        (arg0: fort_myrmidon._fort_myrmidon.Experiment) -> dict[int, str]
        """
    @property
    def MetaDataKeys(self) -> dict[str, bool | int | float | str | Time]:
        """Dict[str,object]: metadata key default value by their unique keys. Object are bool, int, float, str or :class:`Time`
        (arg0: fort_myrmidon._fort_myrmidon.Experiment) -> dict[str, Union[bool, int, float, str, fort_myrmidon._fort_myrmidon.Time]]
        """
    @property
    def Spaces(self) -> dict[int, Space]:
        """Dict[int,Space]: this Experiment space indexed by their SpaceID
        (arg0: fort_myrmidon._fort_myrmidon.Experiment) -> dict[int, fort_myrmidon._fort_myrmidon.Space]
        """

class ExperimentDataInfo:
    """Tracking Data information summary for an Experiment"""
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @property
    def End(self) -> Time:
        """Time: the Time of the first frame available in this Experiement.
        (self: fort_myrmidon._fort_myrmidon.ExperimentDataInfo) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def Frames(self) -> int:
        """int: Total number of Frames accessible in this Experiment.
        (self: fort_myrmidon._fort_myrmidon.ExperimentDataInfo) -> int
        """
    @property
    def Spaces(self) -> dict[int, SpaceDataInfo]:
        """Dict[int,SpaceDataInfo]: the SpaceDataInfo indexed by SpaceId.
        (self: fort_myrmidon._fort_myrmidon.ExperimentDataInfo) -> dict[int, fort_myrmidon._fort_myrmidon.SpaceDataInfo]
        """
    @property
    def Start(self) -> Time:
        """Time: the Time of the first frame available in this Experiement.
        (self: fort_myrmidon._fort_myrmidon.ExperimentDataInfo) -> fort_myrmidon._fort_myrmidon.Time
        """

class FixableError(RuntimeError): ...

class Identification:
    """
    Identifications relates tag to :class:`Ant` with :class:`Time` and geometric
    data.

    Identification can only be created from an :class:`Experiment` with
    :meth:`Experiment.AddIdentification`.

    Identifications are bounded in :class:`Time` in the range
    [ :attr:`Start` , :attr:`End` [. These attributes can respectively be set
    to :meth:`Time.SinceEver` and :meth:`Time.Forever`. Internally
    **fort-myrmidon** ensure the validity of all Identifications. It means
    that:

    * Two Identifications using the same TagValue cannot overlap in Time.
    * Two Identifications using targeting the same :class:`Ant` cannot overlap in Time.

    If any modification through
    :meth:`Experiment.AddIdentification`, :attr:`Start` or :attr:`End` would
    violate one of these conditions, an :class:`OverlappingIdentification`
    exception will be raised.

    Identifications also contains geometric informations on how the
    detected tag is related to the observed :class:`Ant`. These are the
    translation and rotation of the Ant, expressed in the tag coordinate
    reference. Usually, this information is automatically generated from
    the manual measurement of type
    :attr:`Experiment.HEAD_TAIL_MEASUREMENT_TYPE` made in
    **fort-studio**. Alternatively, users can override this behavior by
    setting themselves this pose using
    :meth:`SetUserDefinedAntPose`. :meth:`ClearUserDefinedAntPose` can be
    used to revert to the internally computed pose.

    Note:
        Any angle is measured in radians, with a standard mathematical
        convention. Since in image processing the y-axis is pointing from
        the top of the image to the bottom, positive angles appears
        clockwise.

    Identifications also contains the information of the physical tag size
    used to identify the individual. It can be accessed and set with
    :attr:`TagSize`. The value :attr:`DEFAULT_TAG_SIZE` (i.e. ``0.0``)
    indicates that the :attr:`Experiment.DefaultTagSize` should be
    used. Therefore, for most Ant, this field should be kept to
    :attr:`DEFAULT_TAG_SIZE`, excepted for a few individuals, for
    examples, Queens.

    Attributes:
        DEFAULT_TAG_SIZE (float): a value of ``0.0`` that indicates that the
            :attr:`Experiment.DefaultTagSize` should be used.

    """
    DEFAULT_TAG_SIZE: ClassVar[float] = ...  # read-only
    End: Time
    Start: Time
    TagSize: float
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def ClearUserDefinedAntPose(self) -> None:
        """ClearUserDefinedAntPose(self: fort_myrmidon._fort_myrmidon.Identification) -> None


        Removes any user-defined ant-tag relative geometry data and re-enable
        the one computed from manual measurement made in **fort-studio**.

        """
    def HasDefaultTagSize(self) -> bool:
        """HasDefaultTagSize(self: fort_myrmidon._fort_myrmidon.Identification) -> bool


        Indicates if the :attr:`Experiment.DefaultTagSize` is used

        Returns:
            bool: ``True`` if this Identification uses
            :attr:`Experiment.DefaultTagSize`, i.e. when :attr:`TagSize` ==
            :attr:`DEFAULT_TAG_SIZE`

        """
    def HasUserDefinedAntPose(self) -> bool:
        """HasUserDefinedAntPose(self: fort_myrmidon._fort_myrmidon.Identification) -> bool


        Indicates if the user overrided the computed pose.

        Returns:
            bool: ``True`` if the ant position relatively to the tag has
            been set by the user.

        """
    def SetUserDefinedAntPose(self, antPosition: numpy.ndarray[numpy.float64[2, 1]], antAngle: float) -> None:
        """SetUserDefinedAntPose(self: fort_myrmidon._fort_myrmidon.Identification, antPosition: numpy.ndarray[numpy.float64[2, 1]], antAngle: float) -> None


        Sets an user defined ant position relatively to the tag, overiding
        the one computed from manual measurements.

        To revert to the automatically computed ones, use
        :meth:`ClearUserDefinedAntPose`

        Args:
            antPosition (numpy.ndarray[numpy.float64[2,1]]): the position
                of the ant relatively to the tag center, in the tag
                reference frame
            antAngle (float): the ant angle relatively to the tag angle,
                in radians

        """
    @property
    def AntAngle(self) -> float:
        """float: orientation difference between the ant orientation and its physical tag, expressed in radians.
        (arg0: fort_myrmidon._fort_myrmidon.Identification) -> float
        """
    @property
    def AntPosition(self) -> numpy.ndarray[numpy.float64[2, 1]]:
        """:obj:`numpy.ndarray`: Ant center position relative to the tag center. (float64, size[2,1])
        (arg0: fort_myrmidon._fort_myrmidon.Identification) -> numpy.ndarray[numpy.float64[2, 1]]
        """
    @property
    def TagValue(self) -> int:
        """int: the associated TagID of this identification
        (arg0: fort_myrmidon._fort_myrmidon.Identification) -> int
        """
    @property
    def TargetAntID(self) -> int:
        """int: the associated AntID of this identification
        (arg0: fort_myrmidon._fort_myrmidon.Identification) -> int
        """

class IdentifiedFrame:
    """
    An IdentifiedFrame holds ant detection information associated with
    one video frame.
    """
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def At(self, index: int) -> tuple[int, numpy.ndarray[numpy.float64[3, 1]], numpy.ndarray[numpy.float64[m, 1]]]:
        """At(self: fort_myrmidon._fort_myrmidon.IdentifiedFrame, index: int) -> tuple[int, numpy.ndarray[numpy.float64[3, 1]], numpy.ndarray[numpy.float64[m, 1]]]


        Returns ant information for a given row.

        Args:
            index (int): the index in Positions
        Returns:
            Tuple[int,numpy.ndarray,int]: the AntID, a vector with its (x,y,theta) position, and its current zone.
        Raises:
            IndexError: if index >= len(Positions)

        """
    def Contains(self, antID: int) -> bool:
        """Contains(self: fort_myrmidon._fort_myrmidon.IdentifiedFrame, antID: int) -> bool


        Tests if the frame contains a given antID

        Args:
            antID (int): the AntID to test for.
        Returns:
            bool: True if antID is present in this frame

        """
    @property
    def FrameTime(self) -> Time:
        """Time: acquisition time of the frame
        (self: fort_myrmidon._fort_myrmidon.IdentifiedFrame) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def Height(self) -> int:
        """int: height in pixel of the original video frame
        (self: fort_myrmidon._fort_myrmidon.IdentifiedFrame) -> int
        """
    @property
    def Positions(self) -> numpy.ndarray[numpy.float64[m, n]]:
        """numpy.ndarray: a N row array of (antID,x,y,angle,zone) row vectors for each detected ant in the frame. if Zone is undefined or non-computed, ``zone`` will be 0.
        (arg0: fort_myrmidon._fort_myrmidon.IdentifiedFrame) -> numpy.ndarray[numpy.float64[m, n]]
        """
    @property
    def Space(self) -> int:
        """int: the SpaceID of the Space this frame comes from
        (self: fort_myrmidon._fort_myrmidon.IdentifiedFrame) -> int
        """
    @property
    def Width(self) -> int:
        """int: width in pixel of the original video frame
        (self: fort_myrmidon._fort_myrmidon.IdentifiedFrame) -> int
        """

class Matcher:
    """
    A Matcher helps to build complex :class:`Query` by adding one or
    several constraints.

    Matchers works either on single Ant for trajectory computation, or on
    a pair of Ant when considering interactions. Some matcher have no real
    meaning outside of interaction (i.e. :meth:`InteractionType`) and
    would match any trajectory.

    One would use the following function to get a Matcher :

      * :meth:`AntID` : one of the considered Ant in the trajectory or
        interaction should match a given AntID
      * :meth:`AntMetaData` : one of the key-value meta-data for one of the
        considered Ant should match.
      * :meth:`AntDistanceSmallerThan`, :meth:`AntDistanceGreaterThan` : for
        interaction queries only, ensure some criterion for the distance
        between the two considered ants.
      * :meth:`AntAngleSmallerThan`, :meth:`AntAngleGreaterThan` : for
        interaction queries only, ensure that angle between Ant meets some
        criterion.
      * :meth:`InteractionType` : considers only interaction of a given
        type.
      * :meth:`AntDisplacement`: matches interaction were the displacement
        of either of the Ant is kept under a threshold.

    Using :meth:`And` or :meth:`Or`, one can combine several Matcher
    together to build more complex criterion.
    Examples:

        .. code-block:: python

            import fort_myrmidon as fm
            # m will match ant 001 or 002
            m = fm.Matcher.Or(fm.Matcher.AntID(1),fm.Matcher.AntID(2))

  """
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @staticmethod
    def And(*args) -> Matcher:
        """And(*args) -> fort_myrmidon._fort_myrmidon.Matcher


        Combines several Matcher together in conjuction

        Args:
            *args (Matcher): several other Matcher

        Returns:
            Matcher: a Matcher that matches when all passed matcher also
            matches.

        """
    @staticmethod
    def AntAngleGreaterThan(angle: float) -> Matcher:
        """AntAngleGreaterThan(angle: float) -> fort_myrmidon._fort_myrmidon.Matcher


        A Matcher that matches ant angles.

        In the case of trajectories, it matches anything

        Args:
            angle (float): the angle in radians

        Returns:
            Matcher: a Matcher that matches when the two Ant are facing
            directions which are greater appart than **angle**.

        """
    @staticmethod
    def AntAngleSmallerThan(angle: float) -> Matcher:
        """AntAngleSmallerThan(angle: float) -> fort_myrmidon._fort_myrmidon.Matcher


        A Matcher that matches ant angles.

        In the case of trajectories, it matches anything

        Args:
            angle (float): the angle in radians

        Returns:
            Matcher: a Matcher that matches when the two Ant are facing the
            same direction within **angle**

        """
    @staticmethod
    def AntDisplacement(under: float, minimumGap: Duration = ...) -> Matcher:
        """AntDisplacement(under: float, minimumGap: fort_myrmidon._fort_myrmidon.Duration = 0s) -> fort_myrmidon._fort_myrmidon.Matcher


        A Matcher that rejects large ants displacement.

        Discards any trajectories and interactions where an Ant shows a
        displacement from one detected position to another larger than
        **under**. If **minimumGap** is larger than ``0s``, this check is
        enforced only if there are more than minimumGap time ellapsed between
        two tracked positions.

        Args:
            under (float): maximum allowed Ant displacement in pixels
            minimumGap (Duration): minimum tracking time between positions to
                enable the check.

        Returns:
            Matcher: A Matcher that matches small Ant displacement.

        """
    @staticmethod
    def AntDistanceGreaterThan(distance: float) -> Matcher:
        """AntDistanceGreaterThan(distance: float) -> fort_myrmidon._fort_myrmidon.Matcher


        A Matcher that matches ant distance.

        In the case of trajectories, it matches anything

        Args:
            distance (float): the distance in pixel to match

        Returns:
            Matcher: a Matcher that matches when the two Ant are further than
            **distance**.

        """
    @staticmethod
    def AntDistanceSmallerThan(distance: float) -> Matcher:
        """AntDistanceSmallerThan(distance: float) -> fort_myrmidon._fort_myrmidon.Matcher


        A Matcher that matches ant distance.

        In the case of trajectories, it matches anything

        Args:
            distance (float): the distance in pixel to match

        Returns:
            Matcher: a Matcher that matches when the two Ant are within
            **distance**.

        """
    @staticmethod
    def AntID(antID: int) -> Matcher:
        """AntID(antID: int) -> fort_myrmidon._fort_myrmidon.Matcher


        Matches a given AntID.

        In case of interaction, matches any interaction with one of the Ant
        having **antID**.

        Args:
            antID (int): the AntID to match against.

        Returns:
            Matcher: a matcher that matches **antID**.

        """
    @staticmethod
    def AntMetaData(key: str, value: bool | int | float | str | Time | None) -> Matcher:
        """AntMetaData(key: str, value: Optional[Union[bool, int, float, str, fort_myrmidon._fort_myrmidon.Time]]) -> fort_myrmidon._fort_myrmidon.Matcher


        Matches a given user meta data value.

        In case of interaction, matches any interaction where at least one of
        the Ant meets the criterion.

        Args:
            key (str): the key to match from
            value (bool|int|float|str|fort_myrmidon.Time|None): the value for key. If
                none is passed, it will only works on interaction, and make sure the two
                individuals value for key are the same values.

        Returns:
            Matcher: a Matcher that matches Ant who current **key** meta data
            value matches **value**.

        """
    @overload
    @staticmethod
    def InteractionType(type1: int, type2: int) -> Matcher:
        """InteractionType(type1: int, type2: int) -> fort_myrmidon._fort_myrmidon.Matcher


        Matches InteractionType (type1,type2) and (type2,type1).

        In the case of trajectories it matches anything.

        Args:
            type1 (int): the first AntShapeTypeID to match
            type2 (int): the second AntShapeTypeID to match

        Returns:
            Matcher: A Matcher that matches interactions (type1,type2) or
            (type2,type1).

        """
    @overload
    @staticmethod
    def InteractionType(type1, type2) -> Any:
        """InteractionType(type1: int, type2: int) -> fort_myrmidon._fort_myrmidon.Matcher


        Matches InteractionType (type1,type2) and (type2,type1).

        In the case of trajectories it matches anything.

        Args:
            type1 (int): the first AntShapeTypeID to match
            type2 (int): the second AntShapeTypeID to match

        Returns:
            Matcher: A Matcher that matches interactions (type1,type2) or
            (type2,type1).

        """
    @staticmethod
    def Or(*args) -> Matcher:
        """Or(*args) -> fort_myrmidon._fort_myrmidon.Matcher


        Combines several Matcher together in disjunction

        Args:
            *args (fort_myrmidon.Matcher): several other Matcher

        Returns:
            Matcher: a Matcher that matches when any of the passed matcher
            matches.

        """

class OverlappingIdentification(RuntimeError): ...

class Polygon(Shape):
    """
    Represents a closed polygon.

    Represents a closed polygon from a list of vertices. The polygon is
    always considered closed. i.e. ``[[1,1],[-1,1],[-1,-1],[1,-1]]`` is a
    closed square.

    Note:
        Order matters as ``[[1,1],[-1,-1],[1,-1],[-1,1]]`` represents an
        hourglass.

    Example:
        .. code-block:: python

            square = fort_myrmidon.Polygon(Vertices = [[1,1],[-1,1],[-1,-1],[1,-1]])
            hourglass = fort_myrmidon.Polygon(Vertices = [[1,1],[-1,-1],[1,-1],[-1,1]])
    """
    Vertices: Vector2dList
    @overload
    def __init__(self, Vertices: Vector2dList) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Polygon, Vertices: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.Polygon) -> None
        """
    @overload
    def __init__(self) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Polygon, Vertices: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.Polygon) -> None
        """

class Query:
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @staticmethod
    def CollideFrames(experiment: Experiment, start: Time = ..., end: Time = ..., zoneDepth: int = ..., zoneOrder: ZonePriority = ..., collisionsIgnoreZones: bool = ..., singleThreaded: bool = ..., reportProgress: bool = ..., onEachFrame: Callable[[tuple[IdentifiedFrame, CollisionFrame]], None] | None = ...) -> list | None:
        """CollideFrames(experiment: fort_myrmidon._fort_myrmidon.Experiment, *, start: fort_myrmidon._fort_myrmidon.Time = -∞, end: fort_myrmidon._fort_myrmidon.Time = +∞, zoneDepth: int = 1, zoneOrder: fort_myrmidon._fort_myrmidon.ZonePriority = <ZonePriority.PREDECENCE_LOWER: 0>, collisionsIgnoreZones: bool = False, singleThreaded: bool = False, reportProgress: bool = True, onEachFrame: Optional[Callable[[tuple[fort_myrmidon._fort_myrmidon.IdentifiedFrame, fort_myrmidon._fort_myrmidon.CollisionFrame]], None]] = None) -> Optional[list]


        Gets Ant collision in tracked frames. There is two modes of operation: using a
        onEachFrame callback, that will allow you to perform computation on each tracked
        frame, or using the return value. The latter may require a lot of memory, so it
        will be safer to only query a small time subset.

        Args:
            experiment (Experiment): the experiment to query
            start (Time): the first video acquisition time to consider
            end (Time): the last video acquisition time to consider
            singleThreaded (bool): limits computation to happen in a single thread.
            zoneDepth (int): number of zones that will be computed for each ant.
            zoneOrder (fort_myrmidon.ZonePriority): priority of zone in case of conflict.
            collisionsIgnoreZones (bool): collision detection ignore zones definition
            onNewFrame(Callable[Tuple[fort_myrmidon.IdentifiedFrame,fort_myrmidon.CollisionFrame],None]):
                a callback function to get the result for each frames. If specified,
                this function will return None. It could be used to reduce the memory
                pressure of parsing large datasets.

        Returns:
            List[Tuple[IdentifiedFrame,CollisionFrame]] | None: If **onNewFrame** is `None`, the detected position and collision of the Ants in tracked frames in [ **start** ; **end** [.

        """
    @staticmethod
    def ComputeAntInteractions(experiment: Experiment, start: Time = ..., end: Time = ..., maximumGap: Duration = ..., matcher: Matcher = ..., zoneDepth: int = ..., zoneOrder: ZonePriority = ..., collisionsIgnoreZones: bool = ..., reportFullTrajectories: bool = ..., segmentOnMatcherValueChange: bool = ..., reportSmall: bool = ..., singleThreaded: bool = ..., reportProgress: bool = ..., onNewTrajectory: Callable[[AntTrajectory], None] | None = ..., onNewInteraction: Callable[[AntInteraction], None] | None = ...) -> tuple[list, list] | None:
        '''ComputeAntInteractions(experiment: fort_myrmidon._fort_myrmidon.Experiment, *, start: fort_myrmidon._fort_myrmidon.Time = -∞, end: fort_myrmidon._fort_myrmidon.Time = +∞, maximumGap: fort_myrmidon._fort_myrmidon.Duration = 1s, matcher: fort_myrmidon._fort_myrmidon.Matcher = None, zoneDepth: int = 1, zoneOrder: fort_myrmidon._fort_myrmidon.ZonePriority = <ZonePriority.PREDECENCE_LOWER: 0>, collisionsIgnoreZones: bool = False, reportFullTrajectories: bool = True, segmentOnMatcherValueChange: bool = False, reportSmall: bool = False, singleThreaded: bool = False, reportProgress: bool = True, onNewTrajectory: Optional[Callable[[fort_myrmidon._fort_myrmidon.AntTrajectory], None]] = None, onNewInteraction: Optional[Callable[[fort_myrmidon._fort_myrmidon.AntInteraction], None]] = None) -> Optional[tuple[list, list]]



        Computes Ant Interactions between two times. There is two modes of operation:
        using a **onNewTrajectory** *and/or* **onNewInteraction** callbacks, that will
        allow you to perform computation on each type of result as they are queried, or
        using the return value. The latter may require a lot of memory, so it will be
        safer to only query a small time subset.


        Args:
            experiment (Experiment): the experiment to query
            start (Time): the first video acquisition time to consider
            end (Time): the last video acquisition time to consider
            maximumGap (Duration): maximum tracking gap allowed in
                :class:`AntInteraction` or :class:`AntTrajectory` objects.
            matcher (Matcher): a Matcher that reduces down the query to more specific
                use case.
            zoneDepth (int): number of zones that will be computed for each ant.
            zoneOrder (fort_myrmidon.ZonePriority): priority of zone in case of conflict.
            collisionsIgnoreZones (bool): collision detection ignore zones definition
            reportFullTrajectories (bool): if true, full AntTrajectories
                will be computed and returned. Otherwise, none will be
                returned and only the average Ants position will be
                returned in AntTrajectorySegment.
            singleThreaded (bool): limits computation to happen in a single thread.
            segmentOnMatcherValueChange (bool): if True, when a combined
                matcher ( "behavior" == "grooming" || "behavior" = "sleeping"
                ) value change, create a new trajectory.
            reportSmall (bool): Reports trajectories and interactions with a single time
                point.
            onNewTrajectory (Callable[fort_myrmidon.AntTrajectory,None]): If specified,
                this query will return None, and any discovered trajectory will be
                passed to this callback as they are computed.
            onNewInteraction (Callable[fort_myrmidon.AntInteraction,None]): If
                specified, this query will return None, and any discovered interactions
                will be passed to this callback as they are computed.

        Returns:
            Tuple[List[AntTrajectory],List[AntInteraction]] | None: If neither **onNewTrajectory**, nor **onNewInteraction** is specified, it will return:
                * a list of all AntTrajectory taking place in [start;end[
                  given the matcher criterion and maximumGap if
                  reportFullTrajectories is `true`. Otherwise it will be an
                  empty list.
                * a list of all AntInteraction taking place
                  in [start;end[ given the matcher criterion and maximumGap

        '''
    @staticmethod
    def ComputeAntTrajectories(experiment: Experiment, start: Time = ..., end: Time = ..., maximumGap: Duration = ..., matcher: Matcher = ..., zoneDepth: int = ..., zoneOrder: ZonePriority = ..., segmentOnMatcherValueChange: bool = ..., reportSmall: bool = ..., singleThreaded: bool = ..., reportProgress: bool = ..., onNewTrajectory: Callable[[AntTrajectory], None] | None = ...) -> list | None:
        '''ComputeAntTrajectories(experiment: fort_myrmidon._fort_myrmidon.Experiment, *, start: fort_myrmidon._fort_myrmidon.Time = -∞, end: fort_myrmidon._fort_myrmidon.Time = +∞, maximumGap: fort_myrmidon._fort_myrmidon.Duration = 1s, matcher: fort_myrmidon._fort_myrmidon.Matcher = None, zoneDepth: int = 1, zoneOrder: fort_myrmidon._fort_myrmidon.ZonePriority = <ZonePriority.PREDECENCE_LOWER: 0>, segmentOnMatcherValueChange: bool = False, reportSmall: bool = False, singleThreaded: bool = False, reportProgress: bool = True, onNewTrajectory: Optional[Callable[[fort_myrmidon._fort_myrmidon.AntTrajectory], None]] = None) -> Optional[list]


        Conputes Ant Trajectories between two times. There is two modes of operation:
        using a **onNewTrajectory** callback, that will allow you to perform computation on
        each computed trajectories, or using the return value. The latter may require a
        lot of memory, so it will be safer to only query a small time subset.


        Args:
            experiment (Experiment): the experiment to query
            start (Time): the first video acquisition time to consider
            end (Time): the last video acquisition time to consider
            maximumGap (Duration): maximum tracking gap allowed in a :class:`AntTrajectory` object.
            matcher (Matcher): a :class:`Matcher` that reduces down the query to more specific use case.
            zoneDepth (int): number of zones that will be computed for each ant.
            zoneOrder (fort_myrmidon.ZonePriority): priority of zone in case of conflict.
            singleThreaded (bool): limits computation to happen in a single thread.
            segmentOnMatcherValueChange (bool): if True, when a combined
                matcher ( "behavior" == "grooming" || "behavior" = "sleeping"
                ) value change, create a new trajectory.
            reportSmall (bool): Reports trajectories with a single time point.
            onNewTrajectory (Callable[fort_myrmidon.AntTrajectory,None]): If specified,
                no data will be returned, but this callabled will be called for each
                results. It allows to reduce memory pressure when only a few metrics are
                needed from the results.

        Returns:
            List[AntTrajectory]|None: a list of all :class:`AntTrajectory` taking place in [ **start** ; **end** [ given the **matcher** and **maximumGap** criterions. If **onNewTrajectory** is specified, it will return None.


        '''
    @staticmethod
    def ComputeMeasurementFor(experiment: Experiment, antID: int, measurementTypeID: int) -> list[ComputedMeasurement]:
        """ComputeMeasurementFor(experiment: fort_myrmidon._fort_myrmidon.Experiment, *, antID: int, measurementTypeID: int) -> list[fort_myrmidon._fort_myrmidon.ComputedMeasurement]


        Computes Ant manual measurement in millimeters.

        Computes the list of manual measurements made in `fort-studio` for a
        given Ant in millimeters.

        Args:
            experiment (Experiment): the experiment to query
            antID (int): the Ant to consider
            measurementTypeID (int): the kind of measurement to consider

        Returns:
                List[Measurement]: the list of measurement for **antID** and **measurementTypeID**

        """
    @staticmethod
    def ComputeTagStatistics(experiment: Experiment, fixCorruptedData: bool = ...) -> dict[int, TagStatistics]:
        """ComputeTagStatistics(experiment: fort_myrmidon._fort_myrmidon.Experiment, fixCorruptedData: bool = False) -> dict[int, fort_myrmidon._fort_myrmidon.TagStatistics]


        Computes tag detection statistics in an experiment.

        Args:
            experiment (Experiment): the experiment to query.
            fixCorruptedData (bool): if True will silently fix any data
                corruption error found. This may lead to the loss of large
                chunck of tracking data. Otherwise, a RuntimeError will be
                raised.

        Returns:
            Dict[int,TagStatistics]: the list of TagStatistics indexed by TagID.

        Raises:
            RuntimeError: in vase of data corruption if fixCorruptedData == False

        """
    @staticmethod
    def FindVideoSegments(experiment: Experiment, space: int = ..., start: Time = ..., end: Time = ...) -> VideoSegmentList:
        """FindVideoSegments(experiment: fort_myrmidon._fort_myrmidon.Experiment, *, space: int = 1, start: fort_myrmidon._fort_myrmidon.Time = -∞, end: fort_myrmidon._fort_myrmidon.Time = +∞) -> fort_myrmidon._fort_myrmidon.VideoSegmentList


        Finds :class:`VideoSegment` in a time range

        Args:
            experiment (Experiment): the Experiment to query
            space (int): the SpaceID to ask videos for
            start (Time): the first time to query a video frame
            end (Time): the last time to query a video frame

        Returns:
            VideoSegmentList: list of :class:`VideoSegment` in **space** that covers [**start**;**end**].

        """
    @staticmethod
    def GetDataInformations(experiment: Experiment) -> ExperimentDataInfo:
        """GetDataInformations(experiment: fort_myrmidon._fort_myrmidon.Experiment) -> fort_myrmidon._fort_myrmidon.ExperimentDataInfo"""
    @staticmethod
    def GetMetaDataKeyRanges(experiment: Experiment, key: str, value: bool | int | float | str | Time) -> list[tuple[int, Time, Time]]:
        """GetMetaDataKeyRanges(experiment: fort_myrmidon._fort_myrmidon.Experiment, *, key: str, value: Union[bool, int, float, str, fort_myrmidon._fort_myrmidon.Time]) -> list[tuple[int, fort_myrmidon._fort_myrmidon.Time, fort_myrmidon._fort_myrmidon.Time]]


        Gets the time ranges where metadata key has a given value

        Args:
            experiment (Experiment): the Experiment to query
            key (str): the metadata key to test
            value (str): the value to test for equality

        Returns:
            List[Tuple[int,Time,Time]]: time ranges for each AntID where **key** == **value**

        Raises:
            IndexError: if **key** is not defined in Experiment
            ValueError: if **value** is not the right type for **key**

        """
    @staticmethod
    def GetTagCloseUps(experiment: Experiment, fixCorruptedData: bool = ...) -> object:
        """GetTagCloseUps(experiment: fort_myrmidon._fort_myrmidon.Experiment, fixCorruptedData: bool = False) -> object


        Gets the tag close-up in this experiment

        Args:
            experiment (Experiment): the Experiment to quer
            fixCorruptedData (bool): if True, data corruption will be silently
                fixed. In this case a few close-up may be lost. Otherwise it
                will raise an error.

        Raises:
           RuntimeError: in case of data corruption and if fixCorruptedData == False.

        Returns:
            pandas.DataFrame: the close-up data in the experiment

        """
    @overload
    @staticmethod
    def IdentifyFrames(experiment: Experiment, start: Time = ..., end: Time = ..., singleThreaded: bool = ..., zoneDepth: int = ..., zoneOrder: ZonePriority = ..., reportProgress: bool = ..., onEachFrame: Callable[[IdentifiedFrame], None] | None = ...) -> list | None:
        """IdentifyFrames(experiment: fort_myrmidon._fort_myrmidon.Experiment, *, start: fort_myrmidon._fort_myrmidon.Time = -∞, end: fort_myrmidon._fort_myrmidon.Time = +∞, singleThreaded: bool = False, zoneDepth: int = 1, zoneOrder: fort_myrmidon._fort_myrmidon.ZonePriority = <ZonePriority.PREDECENCE_LOWER: 0>, reportProgress: bool = True, onEachFrame: Optional[Callable[[fort_myrmidon._fort_myrmidon.IdentifiedFrame], None]] = None) -> Optional[list]


        Gets Ant positions in tracked frames.There is two modes of operation: using a
        onEachFrame callback, that will allow you to perform computation on each tracked
        frame, or using the return value. The latter may require a lot of memory, so it
        will be safer to only query a small time subset.

        Args:
            experiment (Experiment): the experiment to query
            start (Time): the first video acquisition time to consider
            end (Time): the last video acquisition time to consider
            singleThreaded (bool): limits computation to happen in a single thread.
            zoneDepth (int): number of zones that will be computed for each ant.
            zoneOrder (fort_myrmidon.ZonePriority): priority of zone in case of conflict.
            onEachFrame (Callable[fort_myrmidon.IdentifiedFrame,None]): a callback
                function for each Identified frames. If specified, IdentifyFrames() will
                return None. If you only care about a few informations, this callback
                can be used to remove memory pressure.

        Returns:
            List[IdentifiedFrame] | None: the detected position of the Ant in video frames in [ **start** ; **end** [ when **onEachFrame** is `None`.

        """
    @overload
    @staticmethod
    def IdentifyFrames() -> Any:
        """IdentifyFrames(experiment: fort_myrmidon._fort_myrmidon.Experiment, *, start: fort_myrmidon._fort_myrmidon.Time = -∞, end: fort_myrmidon._fort_myrmidon.Time = +∞, singleThreaded: bool = False, zoneDepth: int = 1, zoneOrder: fort_myrmidon._fort_myrmidon.ZonePriority = <ZonePriority.PREDECENCE_LOWER: 0>, reportProgress: bool = True, onEachFrame: Optional[Callable[[fort_myrmidon._fort_myrmidon.IdentifiedFrame], None]] = None) -> Optional[list]


        Gets Ant positions in tracked frames.There is two modes of operation: using a
        onEachFrame callback, that will allow you to perform computation on each tracked
        frame, or using the return value. The latter may require a lot of memory, so it
        will be safer to only query a small time subset.

        Args:
            experiment (Experiment): the experiment to query
            start (Time): the first video acquisition time to consider
            end (Time): the last video acquisition time to consider
            singleThreaded (bool): limits computation to happen in a single thread.
            zoneDepth (int): number of zones that will be computed for each ant.
            zoneOrder (fort_myrmidon.ZonePriority): priority of zone in case of conflict.
            onEachFrame (Callable[fort_myrmidon.IdentifiedFrame,None]): a callback
                function for each Identified frames. If specified, IdentifyFrames() will
                return None. If you only care about a few informations, this callback
                can be used to remove memory pressure.

        Returns:
            List[IdentifiedFrame] | None: the detected position of the Ant in video frames in [ **start** ; **end** [ when **onEachFrame** is `None`.

        """

class Shape:
    """
    A Generic class for a Shape
    """

    class Type:
        """Enum for the type of a Shape

        Members:

          CIRCLE : int: a circle

          CAPSULE : int: a capsule

          POLYGON : int: a polygon"""
        __members__: ClassVar[dict] = ...  # read-only
        CAPSULE: ClassVar[Shape.Type] = ...
        CIRCLE: ClassVar[Shape.Type] = ...
        POLYGON: ClassVar[Shape.Type] = ...
        __entries: ClassVar[dict] = ...
        def __init__(self, value: int) -> None:
            """__init__(self: fort_myrmidon._fort_myrmidon.Shape.Type, value: int) -> None"""
        def __eq__(self, other: object) -> bool:
            """__eq__(self: object, other: object) -> bool"""
        def __hash__(self) -> int:
            """__hash__(self: object) -> int"""
        def __index__(self) -> int:
            """__index__(self: fort_myrmidon._fort_myrmidon.Shape.Type) -> int"""
        def __int__(self) -> int:
            """__int__(self: fort_myrmidon._fort_myrmidon.Shape.Type) -> int"""
        def __ne__(self, other: object) -> bool:
            """__ne__(self: object, other: object) -> bool"""
        @property
        def name(self) -> str:
            """name(self: object) -> str

            name(self: object) -> str
            """
        @property
        def value(self) -> int:
            """(arg0: fort_myrmidon._fort_myrmidon.Shape.Type) -> int"""
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @property
    def ShapeType(self):
        """(fort_myrmidon.Shape.Type): the type of the shape
        (arg0: fort_myrmidon._fort_myrmidon.Shape) -> fort::myrmidon::Shape::Type
        """

class ShapeList:
    @overload
    def __init__(self) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: Iterable) -> None
        """
    @overload
    def __init__(self, arg0: ShapeList) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: Iterable) -> None
        """
    @overload
    def __init__(self, arg0: Iterable) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: Iterable) -> None
        """
    def append(self, x: Shape) -> None:
        """append(self: fort_myrmidon._fort_myrmidon.ShapeList, x: fort_myrmidon._fort_myrmidon.Shape) -> None

        Add an item to the end of the list
        """
    def clear(self) -> None:
        """clear(self: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        Clear the contents
        """
    def count(self, x: Shape) -> int:
        """count(self: fort_myrmidon._fort_myrmidon.ShapeList, x: fort_myrmidon._fort_myrmidon.Shape) -> int

        Return the number of times ``x`` appears in the list
        """
    @overload
    def extend(self, L: ShapeList) -> None:
        """extend(*args, **kwargs)
        Overloaded function.

        1. extend(self: fort_myrmidon._fort_myrmidon.ShapeList, L: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        Extend the list by appending all the items in the given list

        2. extend(self: fort_myrmidon._fort_myrmidon.ShapeList, L: Iterable) -> None

        Extend the list by appending all the items in the given list
        """
    @overload
    def extend(self, L: Iterable) -> None:
        """extend(*args, **kwargs)
        Overloaded function.

        1. extend(self: fort_myrmidon._fort_myrmidon.ShapeList, L: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        Extend the list by appending all the items in the given list

        2. extend(self: fort_myrmidon._fort_myrmidon.ShapeList, L: Iterable) -> None

        Extend the list by appending all the items in the given list
        """
    def insert(self, i: int, x: Shape) -> None:
        """insert(self: fort_myrmidon._fort_myrmidon.ShapeList, i: int, x: fort_myrmidon._fort_myrmidon.Shape) -> None

        Insert an item at a given position.
        """
    @overload
    def pop(self) -> Shape:
        """pop(*args, **kwargs)
        Overloaded function.

        1. pop(self: fort_myrmidon._fort_myrmidon.ShapeList) -> fort_myrmidon._fort_myrmidon.Shape

        Remove and return the last item

        2. pop(self: fort_myrmidon._fort_myrmidon.ShapeList, i: int) -> fort_myrmidon._fort_myrmidon.Shape

        Remove and return the item at index ``i``
        """
    @overload
    def pop(self, i: int) -> Shape:
        """pop(*args, **kwargs)
        Overloaded function.

        1. pop(self: fort_myrmidon._fort_myrmidon.ShapeList) -> fort_myrmidon._fort_myrmidon.Shape

        Remove and return the last item

        2. pop(self: fort_myrmidon._fort_myrmidon.ShapeList, i: int) -> fort_myrmidon._fort_myrmidon.Shape

        Remove and return the item at index ``i``
        """
    def remove(self, x: Shape) -> None:
        """remove(self: fort_myrmidon._fort_myrmidon.ShapeList, x: fort_myrmidon._fort_myrmidon.Shape) -> None

        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
    def __bool__(self) -> bool:
        """__bool__(self: fort_myrmidon._fort_myrmidon.ShapeList) -> bool

        Check whether the list is nonempty
        """
    def __contains__(self, x: Shape) -> bool:
        """__contains__(self: fort_myrmidon._fort_myrmidon.ShapeList, x: fort_myrmidon._fort_myrmidon.Shape) -> bool

        Return true the container contains ``x``
        """
    @overload
    def __delitem__(self, arg0: int) -> None:
        """__delitem__(*args, **kwargs)
        Overloaded function.

        1. __delitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: int) -> None

        Delete the list elements at index ``i``

        2. __delitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: slice) -> None

        Delete list elements using a slice object
        """
    @overload
    def __delitem__(self, arg0: slice) -> None:
        """__delitem__(*args, **kwargs)
        Overloaded function.

        1. __delitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: int) -> None

        Delete the list elements at index ``i``

        2. __delitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: slice) -> None

        Delete list elements using a slice object
        """
    def __eq__(self, arg0: ShapeList) -> bool:
        """__eq__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: fort_myrmidon._fort_myrmidon.ShapeList) -> bool"""
    @overload
    def __getitem__(self, s: slice) -> ShapeList:
        """__getitem__(*args, **kwargs)
        Overloaded function.

        1. __getitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, s: slice) -> fort_myrmidon._fort_myrmidon.ShapeList

        Retrieve list elements using a slice object

        2. __getitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: int) -> fort_myrmidon._fort_myrmidon.Shape
        """
    @overload
    def __getitem__(self, arg0: int) -> Shape:
        """__getitem__(*args, **kwargs)
        Overloaded function.

        1. __getitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, s: slice) -> fort_myrmidon._fort_myrmidon.ShapeList

        Retrieve list elements using a slice object

        2. __getitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: int) -> fort_myrmidon._fort_myrmidon.Shape
        """
    def __iter__(self) -> Iterator[Shape]:
        """__iter__(self: fort_myrmidon._fort_myrmidon.ShapeList) -> Iterator[fort_myrmidon._fort_myrmidon.Shape]"""
    def __len__(self) -> int:
        """__len__(self: fort_myrmidon._fort_myrmidon.ShapeList) -> int"""
    def __ne__(self, arg0: ShapeList) -> bool:
        """__ne__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: fort_myrmidon._fort_myrmidon.ShapeList) -> bool"""
    @overload
    def __setitem__(self, arg0: int, arg1: Shape) -> None:
        """__setitem__(*args, **kwargs)
        Overloaded function.

        1. __setitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: int, arg1: fort_myrmidon._fort_myrmidon.Shape) -> None

        2. __setitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: slice, arg1: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        Assign list elements using a slice object
        """
    @overload
    def __setitem__(self, arg0: slice, arg1: ShapeList) -> None:
        """__setitem__(*args, **kwargs)
        Overloaded function.

        1. __setitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: int, arg1: fort_myrmidon._fort_myrmidon.Shape) -> None

        2. __setitem__(self: fort_myrmidon._fort_myrmidon.ShapeList, arg0: slice, arg1: fort_myrmidon._fort_myrmidon.ShapeList) -> None

        Assign list elements using a slice object
        """

class Space:
    """
    An homogenous coordinate system for tracking data.

    A Space represents an homogenous coordinate system for tracking
    data. I.e. coordinates from two different Space cannot be
    compared.

    Spaces are uniquely identified with their :meth:`ID`

    Spaces can only be created from a :class:`Experiment`
    with :meth:`Experiment.CreateSpace`.

    Within a single Space , it could be relevant to define
    :class:`Zone`. These are manipulated with :meth:`CreateZone` and
    :meth:`DeleteZone`.
    """
    Name: str
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def CreateZone(self, name: str) -> Zone:
        """CreateZone(self: fort_myrmidon._fort_myrmidon.Space, name: str) -> fort_myrmidon._fort_myrmidon.Zone


            Creates a new Zone in this space

            Args:
                name (str): the name for the new Zone
            Returns:
                fort_myrmidon.Zone: the newly created Zone

        """
    def DeleteZone(self, zoneID: int) -> None:
        """DeleteZone(self: fort_myrmidon._fort_myrmidon.Space, zoneID: int) -> None


            Deletes a Zone of this Space

            Args:
                zoneID (int): the zoneID in self.Zones to delete
            Raises:
                IndexError: if zoneID is not in self.Zones

        """
    def LocateMovieFrame(self, time: Time) -> tuple[str, int]:
        """LocateMovieFrame(self: fort_myrmidon._fort_myrmidon.Space, time: fort_myrmidon._fort_myrmidon.Time) -> tuple[str, int]


            Locates a movie file and frame for a given time.

            Args:
                time (fort_myrmidon.Time): the time to query for
            Returns:
                str: the absolute file path to the movie file
                int: the movie frame number that was acquired at or just after time
            Raises:
                IndexError: if time is outside of this Space tracking data

        """
    @property
    def ID(self) -> int:
        """(int): the unique SpaceID of this space
        (arg0: fort_myrmidon._fort_myrmidon.Space) -> int
        """
    @property
    def Zones(self) -> dict[int, Zone]:
        """Dict[int,Zone]: the Space's Zone by their ZoneID
        (arg0: fort_myrmidon._fort_myrmidon.Space) -> dict[int, fort_myrmidon._fort_myrmidon.Zone]
        """

class SpaceDataInfo:
    """Tracking Data information summary for a Space."""
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @property
    def End(self) -> Time:
        """Time: the Time of the last frame available in this space.
        (self: fort_myrmidon._fort_myrmidon.SpaceDataInfo) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def Frames(self) -> int:
        """int: Total number of frame found in this Space
        (self: fort_myrmidon._fort_myrmidon.SpaceDataInfo) -> int
        """
    @property
    def Name(self) -> str:
        """The name of the space
        (self: fort_myrmidon._fort_myrmidon.SpaceDataInfo) -> str
        """
    @property
    def Start(self) -> Time:
        """Time: the Time of the first frame available in this space.
        (self: fort_myrmidon._fort_myrmidon.SpaceDataInfo) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def TrackingDataDirectories(self) -> list[TrackingDataDirectoryInfo]:
        """List[TrackingDataDirectoryInfo]: The TrackingDataDirectoryInfo present in this Space
        (self: fort_myrmidon._fort_myrmidon.SpaceDataInfo) -> list[fort_myrmidon._fort_myrmidon.TrackingDataDirectoryInfo]
        """
    @property
    def URI(self) -> str:
        """The internal URI for the Space
        (self: fort_myrmidon._fort_myrmidon.SpaceDataInfo) -> str
        """

class TagFamily:
    """Enumeration of available tag families available in FORT

    Members:

      Tag36h11 : 36h11 family

      Tag36h10 : 36h10 family

      Tag36ARTag : ARTag family

      Tag16h5 : 16h5 family

      Tag25h9 : 25h9 family

      Circle21h7 : Circle21h7 family

      Circle49h12 : Circle49h12 family

      Custom48h12 : Custom48h12 family

      Standard41h12 : Standard41h12 family

      Standard52h13 : Standard52h13 family

      Undefined : Undefined family value"""
    __members__: ClassVar[dict] = ...  # read-only
    Circle21h7: ClassVar[TagFamily] = ...
    Circle49h12: ClassVar[TagFamily] = ...
    Custom48h12: ClassVar[TagFamily] = ...
    Standard41h12: ClassVar[TagFamily] = ...
    Standard52h13: ClassVar[TagFamily] = ...
    Tag16h5: ClassVar[TagFamily] = ...
    Tag25h9: ClassVar[TagFamily] = ...
    Tag36ARTag: ClassVar[TagFamily] = ...
    Tag36h10: ClassVar[TagFamily] = ...
    Tag36h11: ClassVar[TagFamily] = ...
    Undefined: ClassVar[TagFamily] = ...
    __entries: ClassVar[dict] = ...
    def __init__(self, value: int) -> None:
        """__init__(self: fort_myrmidon._fort_myrmidon.TagFamily, value: int) -> None"""
    def __eq__(self, other: object) -> bool:
        """__eq__(self: object, other: object) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: object) -> int"""
    def __index__(self) -> int:
        """__index__(self: fort_myrmidon._fort_myrmidon.TagFamily) -> int"""
    def __int__(self) -> int:
        """__int__(self: fort_myrmidon._fort_myrmidon.TagFamily) -> int"""
    def __ne__(self, other: object) -> bool:
        """__ne__(self: object, other: object) -> bool"""
    @property
    def name(self) -> str:
        """name(self: object) -> str

        name(self: object) -> str
        """
    @property
    def value(self) -> int:
        """(arg0: fort_myrmidon._fort_myrmidon.TagFamily) -> int"""

class TagStatistics:
    """Tag detection statistics for a given TagID"""
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @property
    def Counts(self) -> numpy.ndarray[numpy.uint64[m, 1]]:
        """numpy.ndarray: histogram of detection gaps (int, size[N,1])
        (arg0: fort_myrmidon._fort_myrmidon.TagStatistics) -> numpy.ndarray[numpy.uint64[m, 1]]
        """
    @property
    def FirstSeen(self) -> Time:
        """Time: first time the tag was seen
        (self: fort_myrmidon._fort_myrmidon.TagStatistics) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def ID(self) -> int:
        """int: the TagID it refers to
        (self: fort_myrmidon._fort_myrmidon.TagStatistics) -> int
        """
    @property
    def LastSeen(self) -> Time:
        """Time: last time the tag was seen
        (self: fort_myrmidon._fort_myrmidon.TagStatistics) -> fort_myrmidon._fort_myrmidon.Time
        """

class Time:
    """
    Represents a point in time.

    This object represents a point in time, potentially +/-∞. It
    features operation to compare or measure a Duration between two
    Time.

    The operation manipulating Time objects never modifies the original
    objects, and always return new allocated object, so Time object
    can be considered immuable.

    It also provides methods to convert to and from
    :class:`datetime.datetime` object. In that case these objects
    are considered naïve: expressed in localtime, and ignoring any
    associated timezone information.


    It provides methods to convert to and from :func:`time.time` and
    :meth:`datetime.datetime.timestamp` float values. However for time around
    2020, these only ensure a 10us precision, but Time objects are
    precise to the nanosecond.

    """
    @overload
    def __init__(self) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Time) -> None


        Initialize a Time as the epoch.


        2. __init__(self: fort_myrmidon._fort_myrmidon.Time, timestamp: float) -> None


        Initializes a Time from a float as returned by :func:`time.time` or
        :meth:`datetime.datetime.timestamp`.

        Args:
            timestamp (float): an amount of second since the epoch. Could be
                ``float('inf')`` or ``float('-inf')``.

        Note:
            timestamp are only guaranted to be precise to 10us for Time around
            year 2020.

        """
    @overload
    def __init__(self, timestamp: float) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Time) -> None


        Initialize a Time as the epoch.


        2. __init__(self: fort_myrmidon._fort_myrmidon.Time, timestamp: float) -> None


        Initializes a Time from a float as returned by :func:`time.time` or
        :meth:`datetime.datetime.timestamp`.

        Args:
            timestamp (float): an amount of second since the epoch. Could be
                ``float('inf')`` or ``float('-inf')``.

        Note:
            timestamp are only guaranted to be precise to 10us for Time around
            year 2020.

        """
    def Add(self, d: Duration) -> Time:
        """Add(self: fort_myrmidon._fort_myrmidon.Time, d: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Time


        Adds a Duration to a Time

        Note: `self` remains unchanged.

        Args:
            d (Duration): the Duration to add

        Returns:
            Time: a new Time representing `self + d`

        Raises:
            RuntimeError: if the resulting Time is not representable.

        """
    def After(self, other: Time) -> bool:
        """After(self: fort_myrmidon._fort_myrmidon.Time, other: fort_myrmidon._fort_myrmidon.Time) -> bool


        Tests if this Time is after other

        Similar to `self > other`. `__gt__` operator is also provided.

        Args:
            other (Time): the other Time to test.

        Returns:
            bool:  result of `self > other`

        """
    def Before(self, other: Time) -> bool:
        """Before(self: fort_myrmidon._fort_myrmidon.Time, other: fort_myrmidon._fort_myrmidon.Time) -> bool


        Tests if this Time is before other

        Similar to `self < other`. `__lt__` operator is also provided.

        Args:
            other (Time): the other Time to test.

        Returns:
            bool:  result of `self < other`

        """
    def Equals(self, other: Time) -> bool:
        """Equals(self: fort_myrmidon._fort_myrmidon.Time, other: fort_myrmidon._fort_myrmidon.Time) -> bool


        Tests if this Time is exactly equal to other

        Similar to `self == other`. `__eq__` operator is also provided.

        Args:
            other (Time): the other Time to test.

        Returns:
            bool:  result of `self == other`

        """
    @staticmethod
    def Forever() -> Time:
        """Forever() -> fort_myrmidon._fort_myrmidon.Time


        The positive infinitie time.

        Returns:
            Time: A Time representing +∞

        """
    def IsForever(self) -> bool:
        """IsForever(self: fort_myrmidon._fort_myrmidon.Time) -> bool


        Tests if this Time is +∞

        Returns:
            bool: ``True`` if this time is :meth:`Time.Forever`

        """
    def IsInfinite(self) -> bool:
        """IsInfinite(self: fort_myrmidon._fort_myrmidon.Time) -> bool


        Tests if this Time is + or - ∞

        Returns:
            bool: ``True`` if this time is :meth:`Time.SinceEver` or
            :meth:`Time.Forever`

        """
    def IsSinceEver(self) -> bool:
        """IsSinceEver(self: fort_myrmidon._fort_myrmidon.Time) -> bool


        Tests if this Time is -∞

        Returns:
            bool: ``True`` if this time is :meth:`Time.SinceEver`

        """
    @staticmethod
    def Now() -> Time:
        """Now() -> fort_myrmidon._fort_myrmidon.Time


        Gets the current Time

        Returns:
            Time: the current Time

        """
    @staticmethod
    def Parse(input: str) -> Time:
        """Parse(input: str) -> fort_myrmidon._fort_myrmidon.Time


        Parses a RFC3339 string to a Time.

        Parses a `RFC3339 <https://www.ietf.org/rfc/rfc3339.txt>`_ string
        (i.e. '1970-01-01T00:00:00.000Z') to a Time.

        Args:
            input (str): the string to parse

        Returns:
            fort_myrmidon.Time: a Time that represent input

        Raises:
            Error: if input is a Time that is not representable.

        """
    def Reminder(self, d: Duration) -> Duration:
        """Reminder(self: fort_myrmidon._fort_myrmidon.Time, d: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Duration


        Gets the remaider Duration of self.Round(d)

        Args:
            d (Duration): the duration to round to.

        Returns:
            Duration: the reminder of :meth:`Round(d)`

        """
    def Round(self, d: Duration) -> Time:
        """Round(self: fort_myrmidon._fort_myrmidon.Time, d: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Time


        Rounds a Time to the closest Duration

        Rounds a Time to the closest Duration. Only multiple of seconds and
        power of 10 of Nanosecond smaller than a second are supported.

        Args:
            d (Duration): a multiple of a second or a power of 10 of a
                nanosecond.

        Returns:
            Time: a new Time rounded to d

        Raises:
            ValueError: if d is incorrect

        """
    @staticmethod
    def SinceEver() -> Time:
        """SinceEver() -> fort_myrmidon._fort_myrmidon.Time


        The negative infinite time.

        Returns:
            Time: A Time representing -∞

        """
    def Sub(self, arg0: Time) -> Duration:
        """Sub(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Time) -> fort_myrmidon._fort_myrmidon.Duration


        Measure the Duration between two Time

        Similar to `self - other`. `__sub__` operator is also provided.

        Args:
            other (Time): the other Time to substract.

        Returns:
            bool:  result of `self - other`

        Raises:
            Error: if the result would not fit in a Duration (i.e. if one
                of the :meth:`Time.IsInfinite`)

        """
    def ToTimestamp(self) -> float:
        """ToTimestamp(self: fort_myrmidon._fort_myrmidon.Time) -> float


        Converts to a float as returned by :func:`time.time` or
        :meth:`datetime.datetime.timestamp`

        Returns:
            float: an amount of second since the system's epoch

        """
    @overload
    def __add__(self, arg0: Duration) -> Time:
        """__add__(*args, **kwargs)
        Overloaded function.

        1. __add__(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Time

        2. __add__(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Time
        """
    @overload
    def __add__(self, arg0: Duration) -> Time:
        """__add__(*args, **kwargs)
        Overloaded function.

        1. __add__(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Time

        2. __add__(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Duration) -> fort_myrmidon._fort_myrmidon.Time
        """
    def __eq__(self, arg0: Time) -> bool:
        """__eq__(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Time) -> bool"""
    def __ge__(self, arg0: Time) -> bool:
        """__ge__(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Time) -> bool"""
    def __gt__(self, arg0: Time) -> bool:
        """__gt__(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Time) -> bool"""
    def __le__(self, arg0: Time) -> bool:
        """__le__(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Time) -> bool"""
    def __lt__(self, arg0: Time) -> bool:
        """__lt__(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Time) -> bool"""
    def __sub__(self, arg0: Time) -> Duration:
        """__sub__(self: fort_myrmidon._fort_myrmidon.Time, arg0: fort_myrmidon._fort_myrmidon.Time) -> fort_myrmidon._fort_myrmidon.Duration"""

class TrackingDataDirectoryInfo:
    """Tracking Data informations summary for a Tracking Data Directory."""
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    @property
    def AbsoluteFilePath(self) -> str:
        """str: Absolute filepath of the Tracking Data Directory on the system
        (self: fort_myrmidon._fort_myrmidon.TrackingDataDirectoryInfo) -> str
        """
    @property
    def End(self) -> Time:
        """Time: The Time plus a nanosecond, of the last frame found in This Tracking Data Directory
        (self: fort_myrmidon._fort_myrmidon.TrackingDataDirectoryInfo) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def Frames(self) -> int:
        """int: Number of frames found in this Tracking Data Directory
        (self: fort_myrmidon._fort_myrmidon.TrackingDataDirectoryInfo) -> int
        """
    @property
    def Start(self) -> Time:
        """Time: The Time of the first frame found in this Tracking Data Directory.
        (self: fort_myrmidon._fort_myrmidon.TrackingDataDirectoryInfo) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def URI(self) -> str:
        """str: The internal URI for the Tracking Data Directory
        (self: fort_myrmidon._fort_myrmidon.TrackingDataDirectoryInfo) -> str
        """

class TrackingSolver:
    """
    A TrackingSolver is used to Identify and Collide raw tracking data
    """
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def CollideFrame(self, frame: IdentifiedFrame) -> CollisionFrame:
        """CollideFrame(self: fort_myrmidon._fort_myrmidon.TrackingSolver, frame: fort_myrmidon._fort_myrmidon.IdentifiedFrame) -> fort_myrmidon._fort_myrmidon.CollisionFrame


            Runs Ant zone detection and collision detection on an IdentifiedFrame

            Args:
                frame (fort_myrmidon.IdentifiedFrame): the IdentifiedFrame
                    containing position data for the Ant. It will be modified
                    to hold the current detected zone for the Ant.
            Returns:
                py_fort_myrmidion.CollisionFrame: collision founds for the
                    IdentifiedFrame

        """
    def IdentifyAnt(self, tagID: int, time: Time) -> int:
        """IdentifyAnt(self: fort_myrmidon._fort_myrmidon.TrackingSolver, tagID: int, time: fort_myrmidon._fort_myrmidon.Time) -> int


            Identifies an Ant from a tagID at a given time

            Args:
                tagID (int): the tagID we want to identify for
                time (fort_myrmidon.Time): the time at wich we want to identify

            Returns:
                int: the AntID which is identified by tagID at time, or 0 if
                    no ant is identified by tagID at time.

        """
    def IdentifyFrame(self, frameReadout: object, spaceID: int, zoneDepth: int = ..., zoneOrder: ZonePriority = ...) -> IdentifiedFrame:
        """IdentifyFrame(self: fort_myrmidon._fort_myrmidon.TrackingSolver, frameReadout: object, spaceID: int, zoneDepth: int = 1, zoneOrder: fort_myrmidon._fort_myrmidon.ZonePriority = <ZonePriority.PREDECENCE_LOWER: 0>) -> fort_myrmidon._fort_myrmidon.IdentifiedFrame


            Identifies Ant in a raw frame readout

            Args:
                frame (py_fort_hermes.FrameReadout): the raw data to identify
                spaceID (int): the SpaceID associated with frame
                zoneDepth (int): the maximal number of zone to compute.
                zoneOrder(fort_myrmidon.ZonePriority): specifies if lower zone ID takes
                    predecence over higher zoneID (default) or the opposite.

            Returns:

                fort_myrmidon.IdentifiedFrame: the Ant Identification
                    without zone detection.

        """

class Value:
    @overload
    def __init__(self, arg0: bool) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: bool) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: int) -> None

        3. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: float) -> None

        4. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: str) -> None

        5. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: fort_myrmidon._fort_myrmidon.Time) -> None
        """
    @overload
    def __init__(self, arg0: int) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: bool) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: int) -> None

        3. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: float) -> None

        4. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: str) -> None

        5. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: fort_myrmidon._fort_myrmidon.Time) -> None
        """
    @overload
    def __init__(self, arg0: float) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: bool) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: int) -> None

        3. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: float) -> None

        4. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: str) -> None

        5. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: fort_myrmidon._fort_myrmidon.Time) -> None
        """
    @overload
    def __init__(self, arg0: str) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: bool) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: int) -> None

        3. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: float) -> None

        4. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: str) -> None

        5. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: fort_myrmidon._fort_myrmidon.Time) -> None
        """
    @overload
    def __init__(self, arg0: Time) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: bool) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: int) -> None

        3. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: float) -> None

        4. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: str) -> None

        5. __init__(self: fort_myrmidon._fort_myrmidon.Value, arg0: fort_myrmidon._fort_myrmidon.Time) -> None
        """

class ValueType:
    """Members:

      BOOL

      INT

      DOUBLE

      STRING

      TIME"""
    __members__: ClassVar[dict] = ...  # read-only
    BOOL: ClassVar[ValueType] = ...
    DOUBLE: ClassVar[ValueType] = ...
    INT: ClassVar[ValueType] = ...
    STRING: ClassVar[ValueType] = ...
    TIME: ClassVar[ValueType] = ...
    __entries: ClassVar[dict] = ...
    def __init__(self, value: int) -> None:
        """__init__(self: fort_myrmidon._fort_myrmidon.ValueType, value: int) -> None"""
    def __eq__(self, other: object) -> bool:
        """__eq__(self: object, other: object) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: object) -> int"""
    def __index__(self) -> int:
        """__index__(self: fort_myrmidon._fort_myrmidon.ValueType) -> int"""
    def __int__(self) -> int:
        """__int__(self: fort_myrmidon._fort_myrmidon.ValueType) -> int"""
    def __ne__(self, other: object) -> bool:
        """__ne__(self: object, other: object) -> bool"""
    @property
    def name(self) -> str:
        """name(self: object) -> str

        name(self: object) -> str
        """
    @property
    def value(self) -> int:
        """(arg0: fort_myrmidon._fort_myrmidon.ValueType) -> int"""

class Vector2dList:
    @overload
    def __init__(self) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: Iterable) -> None
        """
    @overload
    def __init__(self, arg0: Vector2dList) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: Iterable) -> None
        """
    @overload
    def __init__(self, arg0: Iterable) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: Iterable) -> None
        """
    def append(self, x: numpy.ndarray[numpy.float64[2, 1]]) -> None:
        """append(self: fort_myrmidon._fort_myrmidon.Vector2dList, x: numpy.ndarray[numpy.float64[2, 1]]) -> None

        Add an item to the end of the list
        """
    def clear(self) -> None:
        """clear(self: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        Clear the contents
        """
    def count(self, x: numpy.ndarray[numpy.float64[2, 1]]) -> int:
        """count(self: fort_myrmidon._fort_myrmidon.Vector2dList, x: numpy.ndarray[numpy.float64[2, 1]]) -> int

        Return the number of times ``x`` appears in the list
        """
    @overload
    def extend(self, L: Vector2dList) -> None:
        """extend(*args, **kwargs)
        Overloaded function.

        1. extend(self: fort_myrmidon._fort_myrmidon.Vector2dList, L: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        Extend the list by appending all the items in the given list

        2. extend(self: fort_myrmidon._fort_myrmidon.Vector2dList, L: Iterable) -> None

        Extend the list by appending all the items in the given list
        """
    @overload
    def extend(self, L: Iterable) -> None:
        """extend(*args, **kwargs)
        Overloaded function.

        1. extend(self: fort_myrmidon._fort_myrmidon.Vector2dList, L: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        Extend the list by appending all the items in the given list

        2. extend(self: fort_myrmidon._fort_myrmidon.Vector2dList, L: Iterable) -> None

        Extend the list by appending all the items in the given list
        """
    def insert(self, i: int, x: numpy.ndarray[numpy.float64[2, 1]]) -> None:
        """insert(self: fort_myrmidon._fort_myrmidon.Vector2dList, i: int, x: numpy.ndarray[numpy.float64[2, 1]]) -> None

        Insert an item at a given position.
        """
    @overload
    def pop(self) -> numpy.ndarray[numpy.float64[2, 1]]:
        """pop(*args, **kwargs)
        Overloaded function.

        1. pop(self: fort_myrmidon._fort_myrmidon.Vector2dList) -> numpy.ndarray[numpy.float64[2, 1]]

        Remove and return the last item

        2. pop(self: fort_myrmidon._fort_myrmidon.Vector2dList, i: int) -> numpy.ndarray[numpy.float64[2, 1]]

        Remove and return the item at index ``i``
        """
    @overload
    def pop(self, i: int) -> numpy.ndarray[numpy.float64[2, 1]]:
        """pop(*args, **kwargs)
        Overloaded function.

        1. pop(self: fort_myrmidon._fort_myrmidon.Vector2dList) -> numpy.ndarray[numpy.float64[2, 1]]

        Remove and return the last item

        2. pop(self: fort_myrmidon._fort_myrmidon.Vector2dList, i: int) -> numpy.ndarray[numpy.float64[2, 1]]

        Remove and return the item at index ``i``
        """
    def remove(self, x: numpy.ndarray[numpy.float64[2, 1]]) -> None:
        """remove(self: fort_myrmidon._fort_myrmidon.Vector2dList, x: numpy.ndarray[numpy.float64[2, 1]]) -> None

        Remove the first item from the list whose value is x. It is an error if there is no such item.
        """
    def __bool__(self) -> bool:
        """__bool__(self: fort_myrmidon._fort_myrmidon.Vector2dList) -> bool

        Check whether the list is nonempty
        """
    def __contains__(self, x: numpy.ndarray[numpy.float64[2, 1]]) -> bool:
        """__contains__(self: fort_myrmidon._fort_myrmidon.Vector2dList, x: numpy.ndarray[numpy.float64[2, 1]]) -> bool

        Return true the container contains ``x``
        """
    @overload
    def __delitem__(self, arg0: int) -> None:
        """__delitem__(*args, **kwargs)
        Overloaded function.

        1. __delitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: int) -> None

        Delete the list elements at index ``i``

        2. __delitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: slice) -> None

        Delete list elements using a slice object
        """
    @overload
    def __delitem__(self, arg0: slice) -> None:
        """__delitem__(*args, **kwargs)
        Overloaded function.

        1. __delitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: int) -> None

        Delete the list elements at index ``i``

        2. __delitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: slice) -> None

        Delete list elements using a slice object
        """
    def __eq__(self, arg0: Vector2dList) -> bool:
        """__eq__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: fort_myrmidon._fort_myrmidon.Vector2dList) -> bool"""
    @overload
    def __getitem__(self, s: slice) -> Vector2dList:
        """__getitem__(*args, **kwargs)
        Overloaded function.

        1. __getitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, s: slice) -> fort_myrmidon._fort_myrmidon.Vector2dList

        Retrieve list elements using a slice object

        2. __getitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: int) -> numpy.ndarray[numpy.float64[2, 1]]
        """
    @overload
    def __getitem__(self, arg0: int) -> numpy.ndarray[numpy.float64[2, 1]]:
        """__getitem__(*args, **kwargs)
        Overloaded function.

        1. __getitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, s: slice) -> fort_myrmidon._fort_myrmidon.Vector2dList

        Retrieve list elements using a slice object

        2. __getitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: int) -> numpy.ndarray[numpy.float64[2, 1]]
        """
    def __iter__(self) -> Iterator[numpy.ndarray[numpy.float64[2, 1]]]:
        """__iter__(self: fort_myrmidon._fort_myrmidon.Vector2dList) -> Iterator[numpy.ndarray[numpy.float64[2, 1]]]"""
    def __len__(self) -> int:
        """__len__(self: fort_myrmidon._fort_myrmidon.Vector2dList) -> int"""
    def __ne__(self, arg0: Vector2dList) -> bool:
        """__ne__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: fort_myrmidon._fort_myrmidon.Vector2dList) -> bool"""
    @overload
    def __setitem__(self, arg0: int, arg1: numpy.ndarray[numpy.float64[2, 1]]) -> None:
        """__setitem__(*args, **kwargs)
        Overloaded function.

        1. __setitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: int, arg1: numpy.ndarray[numpy.float64[2, 1]]) -> None

        2. __setitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: slice, arg1: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        Assign list elements using a slice object
        """
    @overload
    def __setitem__(self, arg0: slice, arg1: Vector2dList) -> None:
        """__setitem__(*args, **kwargs)
        Overloaded function.

        1. __setitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: int, arg1: numpy.ndarray[numpy.float64[2, 1]]) -> None

        2. __setitem__(self: fort_myrmidon._fort_myrmidon.Vector2dList, arg0: slice, arg1: fort_myrmidon._fort_myrmidon.Vector2dList) -> None

        Assign list elements using a slice object
        """

class VideoFrameData:
    """
    The tracking data and query results associated with a Video frame.

    Note:
        After a call to :meth:`Query.FindMovieSegments` this structure will
        contain no matched data. One must call :meth:`VideoSequence.Match`
        to associate query results with the :class:`VideoFrameData`
        present in the :class:`VideoSegmentList`.

    Warning:
        In the unlikely case of a :class:`VideoFrameData` without any
        tracking data ( the movie frame was exported but no tracking was
        reported ), the value of :attr:`VideoFrameData.Time` will be set
        to :meth:`Time.SinceEver` and all other query result field will be
        set to `None`.
    """
    def __init__(self, position: int, time: Time) -> None:
        """__init__(self: fort_myrmidon._fort_myrmidon.VideoFrameData, position: int, time: fort_myrmidon._fort_myrmidon.Time) -> None"""
    def Empty(self) -> bool:
        """Empty(self: fort_myrmidon._fort_myrmidon.VideoFrameData) -> bool


        Indicates the (unlikely) case where no tracking data is associated
        with this video frame.

        Returns:
            bool: `True` if there is no tracking data (even a timeout / frame drop report) associated with this video frame.

        """
    @property
    def Collided(self) -> CollisionFrame:
        """CollisionFrame: the ants collision (if previously matched)
        (self: fort_myrmidon._fort_myrmidon.VideoFrameData) -> fort_myrmidon._fort_myrmidon.CollisionFrame
        """
    @property
    def Identified(self) -> IdentifiedFrame:
        """IdentifiedFrame: the ants position (if previously matched)
        (self: fort_myrmidon._fort_myrmidon.VideoFrameData) -> fort_myrmidon._fort_myrmidon.IdentifiedFrame
        """
    @property
    def Interactions(self) -> list[AntInteraction]:
        """List[AntInteraction]: the interactions in this frame (if previously matched)
        (self: fort_myrmidon._fort_myrmidon.VideoFrameData) -> list[fort_myrmidon._fort_myrmidon.AntInteraction]
        """
    @property
    def Position(self) -> int:
        """int: the frame position in the video file
        (self: fort_myrmidon._fort_myrmidon.VideoFrameData) -> int
        """
    @property
    def Time(self) -> Time:
        """Time: the video frame acquisition time (if available)
        (self: fort_myrmidon._fort_myrmidon.VideoFrameData) -> fort_myrmidon._fort_myrmidon.Time
        """
    @property
    def Trajectories(self) -> list[AntTrajectory]:
        """List[AntTrajectory]: the trajectories in this frame (if previously matched)
        (self: fort_myrmidon._fort_myrmidon.VideoFrameData) -> list[fort_myrmidon._fort_myrmidon.AntTrajectory]
        """

class VideoFrameDataList:
    @overload
    def __init__(self) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: Iterable) -> None
        """
    @overload
    def __init__(self, arg0: VideoFrameDataList) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: Iterable) -> None
        """
    @overload
    def __init__(self, arg0: Iterable) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: Iterable) -> None
        """
    def append(self, x: VideoFrameData) -> None:
        """append(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, x: fort_myrmidon._fort_myrmidon.VideoFrameData) -> None

        Add an item to the end of the list
        """
    def clear(self) -> None:
        """clear(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        Clear the contents
        """
    @overload
    def extend(self, L: VideoFrameDataList) -> None:
        """extend(*args, **kwargs)
        Overloaded function.

        1. extend(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, L: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        Extend the list by appending all the items in the given list

        2. extend(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, L: Iterable) -> None

        Extend the list by appending all the items in the given list
        """
    @overload
    def extend(self, L: Iterable) -> None:
        """extend(*args, **kwargs)
        Overloaded function.

        1. extend(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, L: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        Extend the list by appending all the items in the given list

        2. extend(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, L: Iterable) -> None

        Extend the list by appending all the items in the given list
        """
    def insert(self, i: int, x: VideoFrameData) -> None:
        """insert(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, i: int, x: fort_myrmidon._fort_myrmidon.VideoFrameData) -> None

        Insert an item at a given position.
        """
    @overload
    def pop(self) -> VideoFrameData:
        """pop(*args, **kwargs)
        Overloaded function.

        1. pop(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> fort_myrmidon._fort_myrmidon.VideoFrameData

        Remove and return the last item

        2. pop(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, i: int) -> fort_myrmidon._fort_myrmidon.VideoFrameData

        Remove and return the item at index ``i``
        """
    @overload
    def pop(self, i: int) -> VideoFrameData:
        """pop(*args, **kwargs)
        Overloaded function.

        1. pop(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> fort_myrmidon._fort_myrmidon.VideoFrameData

        Remove and return the last item

        2. pop(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, i: int) -> fort_myrmidon._fort_myrmidon.VideoFrameData

        Remove and return the item at index ``i``
        """
    def __bool__(self) -> bool:
        """__bool__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> bool

        Check whether the list is nonempty
        """
    @overload
    def __delitem__(self, arg0: int) -> None:
        """__delitem__(*args, **kwargs)
        Overloaded function.

        1. __delitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: int) -> None

        Delete the list elements at index ``i``

        2. __delitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: slice) -> None

        Delete list elements using a slice object
        """
    @overload
    def __delitem__(self, arg0: slice) -> None:
        """__delitem__(*args, **kwargs)
        Overloaded function.

        1. __delitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: int) -> None

        Delete the list elements at index ``i``

        2. __delitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: slice) -> None

        Delete list elements using a slice object
        """
    @overload
    def __getitem__(self, s: slice) -> VideoFrameDataList:
        """__getitem__(*args, **kwargs)
        Overloaded function.

        1. __getitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, s: slice) -> fort_myrmidon._fort_myrmidon.VideoFrameDataList

        Retrieve list elements using a slice object

        2. __getitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: int) -> fort_myrmidon._fort_myrmidon.VideoFrameData
        """
    @overload
    def __getitem__(self, arg0: int) -> VideoFrameData:
        """__getitem__(*args, **kwargs)
        Overloaded function.

        1. __getitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, s: slice) -> fort_myrmidon._fort_myrmidon.VideoFrameDataList

        Retrieve list elements using a slice object

        2. __getitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: int) -> fort_myrmidon._fort_myrmidon.VideoFrameData
        """
    def __iter__(self) -> Iterator[VideoFrameData]:
        """__iter__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> Iterator[fort_myrmidon._fort_myrmidon.VideoFrameData]"""
    def __len__(self) -> int:
        """__len__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> int"""
    @overload
    def __setitem__(self, arg0: int, arg1: VideoFrameData) -> None:
        """__setitem__(*args, **kwargs)
        Overloaded function.

        1. __setitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: int, arg1: fort_myrmidon._fort_myrmidon.VideoFrameData) -> None

        2. __setitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: slice, arg1: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        Assign list elements using a slice object
        """
    @overload
    def __setitem__(self, arg0: slice, arg1: VideoFrameDataList) -> None:
        """__setitem__(*args, **kwargs)
        Overloaded function.

        1. __setitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: int, arg1: fort_myrmidon._fort_myrmidon.VideoFrameData) -> None

        2. __setitem__(self: fort_myrmidon._fort_myrmidon.VideoFrameDataList, arg0: slice, arg1: fort_myrmidon._fort_myrmidon.VideoFrameDataList) -> None

        Assign list elements using a slice object
        """

class VideoSegment:
    '''

    A VideoSegment represents a part of a Video file with its associated
    tracking data.

    VideoSegment are most often queried with
    :meth:`Query.FindVideoSegments`. Once queried, they are blank, i.e. no
    query results will appears in :attr:`VideoSegment.Data`. One would
    call :meth:`Match` to associate queries results with these
    segments. Finally a :class:`VideoSequence` context manager can be used
    to iterate over each video frame of the :class:`VideoSegmentList`.

    Note:
        :meth:`Query.FindVideoSegment`, :meth:`Match` and
        :class:`VideoSequence` use a :class:`VideoSegmentList` as return
        value or arguments. Indeed it could happen that the desired
        sequence of images span over multiple video file.

    Example:
        .. code-block:: python

            import fort_myrmidon as fm

            e = fm.Experiment.Open("file.myrmidon")

            # note it would be extremly computationally intensive to
            # iterate over the whole experiment, we select a time region.
            start = fm.Time.Parse(\'2019-11-02T20:00:00.000Z\')
            end = start.Add(30 * fm.Duration.Second)

            ## step 0: make some query on the experiment
            trajectories = fm.Query.ComputeAntTrajectories(e,start=start,end=end)

            ## step 1: look up segments bet
            segments = fm.Query.FindVideoSegments(e,
                                                  space = 1,
                                                  start = start,
                                                  end = end)

            ## step 2: match queried data with video frames
            fm.VideoSegment.Match(segments,trajectories)

            ## step 3: iterate over all video frame and matched data
            with fm.VideoSequence(segments) as sequence:
                for frame,data in sequence:
                    ## step 3.1 on each cv2.Mat `frame` do something based
                    ## on `data`
                    pass

    '''
    Data: VideoFrameDataList
    End: int
    def __init__(self, arg0: int) -> None:
        """__init__(self: fort_myrmidon._fort_myrmidon.VideoSegment, arg0: int) -> None"""
    @overload
    @staticmethod
    def Match(segments: VideoSegmentList, identifiedFrames: list[IdentifiedFrame]) -> None:
        """Match(*args, **kwargs)
        Overloaded function.

        1. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, identifiedFrames: list[fort_myrmidon._fort_myrmidon.IdentifiedFrame]) -> None


        Matches :class:`IdentifiedFrame` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            identifiedFrames (List[IdentifiedFrame]): the IdentifiedFrames to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        2. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, collisionData: list[tuple[fort_myrmidon._fort_myrmidon.IdentifiedFrame, fort_myrmidon._fort_myrmidon.CollisionFrame]]) -> None


        Matches :class:`CollisionData` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            collisionData (List[CollisionData]): the CollisionData to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        3. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, trajectories: list[fort_myrmidon._fort_myrmidon.AntTrajectory]) -> None


        Matches :class:`AntTrajectory` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            trajectories (List[AntTrajectory]): the AntTrajectories to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        4. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, interactions: list[fort_myrmidon._fort_myrmidon.AntInteraction]) -> None


        Matches :class:`AntInteraction` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            interactions (List[AntInteraction]): the AntInteractions to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`

        """
    @overload
    @staticmethod
    def Match(segments: VideoSegmentList, collisionData: list[tuple[IdentifiedFrame, CollisionFrame]]) -> None:
        """Match(*args, **kwargs)
        Overloaded function.

        1. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, identifiedFrames: list[fort_myrmidon._fort_myrmidon.IdentifiedFrame]) -> None


        Matches :class:`IdentifiedFrame` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            identifiedFrames (List[IdentifiedFrame]): the IdentifiedFrames to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        2. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, collisionData: list[tuple[fort_myrmidon._fort_myrmidon.IdentifiedFrame, fort_myrmidon._fort_myrmidon.CollisionFrame]]) -> None


        Matches :class:`CollisionData` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            collisionData (List[CollisionData]): the CollisionData to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        3. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, trajectories: list[fort_myrmidon._fort_myrmidon.AntTrajectory]) -> None


        Matches :class:`AntTrajectory` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            trajectories (List[AntTrajectory]): the AntTrajectories to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        4. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, interactions: list[fort_myrmidon._fort_myrmidon.AntInteraction]) -> None


        Matches :class:`AntInteraction` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            interactions (List[AntInteraction]): the AntInteractions to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`

        """
    @overload
    @staticmethod
    def Match(segments: VideoSegmentList, trajectories: list[AntTrajectory]) -> None:
        """Match(*args, **kwargs)
        Overloaded function.

        1. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, identifiedFrames: list[fort_myrmidon._fort_myrmidon.IdentifiedFrame]) -> None


        Matches :class:`IdentifiedFrame` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            identifiedFrames (List[IdentifiedFrame]): the IdentifiedFrames to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        2. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, collisionData: list[tuple[fort_myrmidon._fort_myrmidon.IdentifiedFrame, fort_myrmidon._fort_myrmidon.CollisionFrame]]) -> None


        Matches :class:`CollisionData` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            collisionData (List[CollisionData]): the CollisionData to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        3. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, trajectories: list[fort_myrmidon._fort_myrmidon.AntTrajectory]) -> None


        Matches :class:`AntTrajectory` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            trajectories (List[AntTrajectory]): the AntTrajectories to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        4. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, interactions: list[fort_myrmidon._fort_myrmidon.AntInteraction]) -> None


        Matches :class:`AntInteraction` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            interactions (List[AntInteraction]): the AntInteractions to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`

        """
    @overload
    @staticmethod
    def Match(segments: VideoSegmentList, interactions: list[AntInteraction]) -> None:
        """Match(*args, **kwargs)
        Overloaded function.

        1. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, identifiedFrames: list[fort_myrmidon._fort_myrmidon.IdentifiedFrame]) -> None


        Matches :class:`IdentifiedFrame` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            identifiedFrames (List[IdentifiedFrame]): the IdentifiedFrames to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        2. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, collisionData: list[tuple[fort_myrmidon._fort_myrmidon.IdentifiedFrame, fort_myrmidon._fort_myrmidon.CollisionFrame]]) -> None


        Matches :class:`CollisionData` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            collisionData (List[CollisionData]): the CollisionData to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        3. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, trajectories: list[fort_myrmidon._fort_myrmidon.AntTrajectory]) -> None


        Matches :class:`AntTrajectory` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            trajectories (List[AntTrajectory]): the AntTrajectories to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`


        4. Match(segments: fort_myrmidon._fort_myrmidon.VideoSegmentList, interactions: list[fort_myrmidon._fort_myrmidon.AntInteraction]) -> None


        Matches :class:`AntInteraction` with a :class:`VideoSegmentList`

        Args:
            segments (VideoSegmentList): the segments to associate data with
            interactions (List[AntInteraction]): the AntInteractions to match with `segments`.

        Raises:
            ValueError: if the all segments are not from the same :class:`Space`

        """
    @property
    def AbsoluteFilePath(self) -> str:
        """str: the absolute filepath to the video file
        (self: fort_myrmidon._fort_myrmidon.VideoSegment) -> str
        """
    @property
    def Begin(self) -> int:
        """int: the first video frame position in the video file corresponding to the segment
        (self: fort_myrmidon._fort_myrmidon.VideoSegment) -> int
        """
    @property
    def Space(self) -> int:
        """int: the :class:`Space` the segment belongs to
        (self: fort_myrmidon._fort_myrmidon.VideoSegment) -> int
        """

class VideoSegmentList:
    """
    An opaque list of :class:`VideoSegment`.

    It works just as a :class:`list` of :class:`VideoSegment`.

    """
    @overload
    def __init__(self) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: Iterable) -> None
        """
    @overload
    def __init__(self, arg0: VideoSegmentList) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: Iterable) -> None
        """
    @overload
    def __init__(self, arg0: Iterable) -> None:
        """__init__(*args, **kwargs)
        Overloaded function.

        1. __init__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        2. __init__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        Copy constructor

        3. __init__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: Iterable) -> None
        """
    def append(self, x: VideoSegment) -> None:
        """append(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, x: fort_myrmidon._fort_myrmidon.VideoSegment) -> None

        Add an item to the end of the list
        """
    def clear(self) -> None:
        """clear(self: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        Clear the contents
        """
    def deepcopy(self) -> VideoSegmentList:
        """deepcopy(self: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> fort_myrmidon._fort_myrmidon.VideoSegmentList


        Performs a deepcopy of the list.

        The main purpose is for the unittest implementation.

        """
    @overload
    def extend(self, L: VideoSegmentList) -> None:
        """extend(*args, **kwargs)
        Overloaded function.

        1. extend(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, L: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        Extend the list by appending all the items in the given list

        2. extend(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, L: Iterable) -> None

        Extend the list by appending all the items in the given list
        """
    @overload
    def extend(self, L: Iterable) -> None:
        """extend(*args, **kwargs)
        Overloaded function.

        1. extend(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, L: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        Extend the list by appending all the items in the given list

        2. extend(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, L: Iterable) -> None

        Extend the list by appending all the items in the given list
        """
    def insert(self, i: int, x: VideoSegment) -> None:
        """insert(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, i: int, x: fort_myrmidon._fort_myrmidon.VideoSegment) -> None

        Insert an item at a given position.
        """
    @overload
    def pop(self) -> VideoSegment:
        """pop(*args, **kwargs)
        Overloaded function.

        1. pop(self: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> fort_myrmidon._fort_myrmidon.VideoSegment

        Remove and return the last item

        2. pop(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, i: int) -> fort_myrmidon._fort_myrmidon.VideoSegment

        Remove and return the item at index ``i``
        """
    @overload
    def pop(self, i: int) -> VideoSegment:
        """pop(*args, **kwargs)
        Overloaded function.

        1. pop(self: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> fort_myrmidon._fort_myrmidon.VideoSegment

        Remove and return the last item

        2. pop(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, i: int) -> fort_myrmidon._fort_myrmidon.VideoSegment

        Remove and return the item at index ``i``
        """
    def __bool__(self) -> bool:
        """__bool__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> bool

        Check whether the list is nonempty
        """
    @overload
    def __delitem__(self, arg0: int) -> None:
        """__delitem__(*args, **kwargs)
        Overloaded function.

        1. __delitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: int) -> None

        Delete the list elements at index ``i``

        2. __delitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: slice) -> None

        Delete list elements using a slice object
        """
    @overload
    def __delitem__(self, arg0: slice) -> None:
        """__delitem__(*args, **kwargs)
        Overloaded function.

        1. __delitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: int) -> None

        Delete the list elements at index ``i``

        2. __delitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: slice) -> None

        Delete list elements using a slice object
        """
    @overload
    def __getitem__(self, s: slice) -> VideoSegmentList:
        """__getitem__(*args, **kwargs)
        Overloaded function.

        1. __getitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, s: slice) -> fort_myrmidon._fort_myrmidon.VideoSegmentList

        Retrieve list elements using a slice object

        2. __getitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: int) -> fort_myrmidon._fort_myrmidon.VideoSegment
        """
    @overload
    def __getitem__(self, arg0: int) -> VideoSegment:
        """__getitem__(*args, **kwargs)
        Overloaded function.

        1. __getitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, s: slice) -> fort_myrmidon._fort_myrmidon.VideoSegmentList

        Retrieve list elements using a slice object

        2. __getitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: int) -> fort_myrmidon._fort_myrmidon.VideoSegment
        """
    def __iter__(self) -> Iterator[VideoSegment]:
        """__iter__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> Iterator[fort_myrmidon._fort_myrmidon.VideoSegment]"""
    def __len__(self) -> int:
        """__len__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> int"""
    @overload
    def __setitem__(self, arg0: int, arg1: VideoSegment) -> None:
        """__setitem__(*args, **kwargs)
        Overloaded function.

        1. __setitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: int, arg1: fort_myrmidon._fort_myrmidon.VideoSegment) -> None

        2. __setitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: slice, arg1: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        Assign list elements using a slice object
        """
    @overload
    def __setitem__(self, arg0: slice, arg1: VideoSegmentList) -> None:
        """__setitem__(*args, **kwargs)
        Overloaded function.

        1. __setitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: int, arg1: fort_myrmidon._fort_myrmidon.VideoSegment) -> None

        2. __setitem__(self: fort_myrmidon._fort_myrmidon.VideoSegmentList, arg0: slice, arg1: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None

        Assign list elements using a slice object
        """

class VideoSequence:
    """
    A contextmanager for iterating over Video Frame and matched data.

    Examples:
        .. code-block:: python

            import fort_myrmidon as fm

            ## will iterate over all video frame of space 1
            segments = fm.Query.FindVideoSegments(e,space = 1)
            with fm.VideoSequence(segments) as sequence:
                for frame, data in sequence:
                    pass

    """
    def __init__(self, arg0: VideoSegmentList) -> None:
        """__init__(self: fort_myrmidon._fort_myrmidon.VideoSequence, arg0: fort_myrmidon._fort_myrmidon.VideoSegmentList) -> None"""
    def __enter__(self) -> VideoSequence:
        """__enter__(self: fort_myrmidon._fort_myrmidon.VideoSequence) -> fort_myrmidon._fort_myrmidon.VideoSequence"""
    def __exit__(self, arg0: object, arg1: object, arg2: object) -> bool:
        """__exit__(self: fort_myrmidon._fort_myrmidon.VideoSequence, arg0: object, arg1: object, arg2: object) -> bool"""
    def __iter__(self) -> VideoSequence:
        """__iter__(self: fort_myrmidon._fort_myrmidon.VideoSequence) -> fort_myrmidon._fort_myrmidon.VideoSequence"""
    def __next__(self) -> tuple:
        """__next__(self: fort_myrmidon._fort_myrmidon.VideoSequence) -> tuple"""

class Zone:
    """
    Defines a named region of interest for tracking and interactions

    Zones defines a named region of interest for tracking and
    interactions. It means that two :class:`Ant`, which are lying in two
    separate Zones will never report a collision or interaction.

    Zones are uniquely identified trough their :meth:`ID` in an
    :class:`Experiment`, but they are managed and are related to a single
    :class:`Space`. They also have a user defined :attr:`Name`, but it is
    never used internally.

    By default an :class:`Ant` lies in no Zone at all, which is identified
    by the ZoneID 0.

    Zone have time valid :class:`ZoneDefinition` which represents their
    geometries. In most cases a Zone will have a single
    :class:`ZoneDefinition` valid for
    ] :func:`Time.SinceEver` ; :func:`Time.Forever` [. However it is possible
    to add as many different ZoneDefinitions to a Zone, as long as they do
    not overlap in Time. The definitions are manipulated with
    :meth:`AddDefinition` and :meth:`DeleteDefinition`.
    """
    Name: str
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""
    def AddDefinition(self, shapes: ShapeList = ..., start: Time = ..., end: Time = ...) -> ZoneDefinition:
        """AddDefinition(self: fort_myrmidon._fort_myrmidon.Zone, shapes: fort_myrmidon._fort_myrmidon.ShapeList = [], start: fort_myrmidon._fort_myrmidon.Time = -∞, end: fort_myrmidon._fort_myrmidon.Time = +∞) -> fort_myrmidon._fort_myrmidon.ZoneDefinition


            Adds a new ZoneDefinition to this Zone

            Args:
                shapes (List[Shape]): the
                    geometry of the ZoneDefinition
                start (Time): the first valid Time for the
                    ZoneDefinition
                end (Time): the first valid Time for the
                    ZoneDefinition

            Returns:
                ZoneDefinition: the new :class:`ZoneDefinition` for this Zone

            Raises:
                ValueError: if start or end would make an overlapping
                    definition with another ZoneDefinition of this Zone

        """
    def DeleteDefinition(self, index: int) -> None:
        """DeleteDefinition(self: fort_myrmidon._fort_myrmidon.Zone, index: int) -> None


            Removes a ZoneDefinition

            Args:
                index (int): the index in :attr:`Definitions` to remove.
            Raises:
                IndexError: if index >= len(self.Definitions)

        """
    @property
    def Definitions(self) -> list[ZoneDefinition]:
        """List[ZoneDefinition]: the definitions for this Zone
        (arg0: fort_myrmidon._fort_myrmidon.Zone) -> list[fort_myrmidon._fort_myrmidon.ZoneDefinition]
        """
    @property
    def ID(self) -> int:
        """int: the unique ID for this Zone
        (arg0: fort_myrmidon._fort_myrmidon.Zone) -> int
        """

class ZoneDefinition:
    """Defines the geometry of a :class:`Zone` during a time interval"""
    End: Time
    Shapes: ShapeList
    Start: Time
    def __init__(self, *args, **kwargs) -> None:
        """Initialize self.  See help(type(self)) for accurate signature."""

class ZonePriority:
    """Members:

      PREDECENCE_HIGHER : Zone with Higher ZoneID will take predecence over lower ID zones

      PREDECENCE_LOWER : Zone with lower ZoneID will take predecence over higher ID zones"""
    __members__: ClassVar[dict] = ...  # read-only
    PREDECENCE_HIGHER: ClassVar[ZonePriority] = ...
    PREDECENCE_LOWER: ClassVar[ZonePriority] = ...
    __entries: ClassVar[dict] = ...
    def __init__(self, value: int) -> None:
        """__init__(self: fort_myrmidon._fort_myrmidon.ZonePriority, value: int) -> None"""
    def __eq__(self, other: object) -> bool:
        """__eq__(self: object, other: object) -> bool"""
    def __hash__(self) -> int:
        """__hash__(self: object) -> int"""
    def __index__(self) -> int:
        """__index__(self: fort_myrmidon._fort_myrmidon.ZonePriority) -> int"""
    def __int__(self) -> int:
        """__int__(self: fort_myrmidon._fort_myrmidon.ZonePriority) -> int"""
    def __ne__(self, other: object) -> bool:
        """__ne__(self: object, other: object) -> bool"""
    @property
    def name(self) -> str:
        """name(self: object) -> str

        name(self: object) -> str
        """
    @property
    def value(self) -> int:
        """(arg0: fort_myrmidon._fort_myrmidon.ZonePriority) -> int"""

def DefaultPalette() -> list[tuple[int, int, int]]:
    """DefaultPalette() -> list[tuple[int, int, int]]


    Returns **fort-myrmidon** default palette.

    In **fort_myrmidon**, a Color is simply a Tuple[:obj:`int`,
    :obj:`int`, :obj:`int`].  The default palette has 7 color-blind
    friendly colors.

    Returns:
        List[Tuple[int,int,int]]: 7 color-blind friendly colors.

    """
def DefaultPaletteColor(index: int) -> tuple[int, int, int]:
    """DefaultPaletteColor(index: int) -> tuple[int, int, int]


    Safely returns one of the :func:`DefaultPalette` color for any index.

    Args:
        index(int) : a positive index

    Returns:
        Tuple[int,int,int]: a color from :func:`DefaultPalette` wrapping
            arround **index**.

    """
def FormatAntID(antID: int) -> str:
    """FormatAntID(antID: int) -> str


        Formats an AntID to the conventional format.

        Args:
            antID (int): the AntID to format
        Returns:
            str: antID formatted in a string

    """
def FormatTagID(tagID: int) -> str:
    """FormatTagID(tagID: int) -> str


        Formats a TagID to the conventional format.

        Args:
            tagID (int): the TagID to format
        Returns:
            str: tagID formatted in a string

    """
