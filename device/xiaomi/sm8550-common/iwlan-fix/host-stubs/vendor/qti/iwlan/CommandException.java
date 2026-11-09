package vendor.qti.iwlan; public class CommandException extends RuntimeException {public enum Error{RADIO_NOT_AVAILABLE}public final Error error;public CommandException(Error e){error=e;}}
