type memCache;

[@bs.deriving abstract]
type memCacheOptions = {
  [@bs.optional]
  size: int,
  [@bs.optional]
  ttl: int,
  [@bs.optional]
  initialState: Js.Json.t,
};

[@bs.module "graphql-hooks-memcache"] [@bs.new]
external memCache: memCacheOptions => memCache = "default";

let make = (~size=?, ~ttl=?, ~initialState=?, ()) => {
  let options = memCacheOptions(~size?, ~ttl?, ~initialState?, ());
  memCache(options);
};