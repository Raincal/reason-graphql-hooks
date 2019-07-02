type memCache;

[@bs.deriving abstract]
type options = {
  [@bs.optional]
  size: int,
  [@bs.optional]
  ttl: int,
  [@bs.optional]
  initialState: Js.Json.t,
};

[@bs.module "graphql-hooks-memcache"] [@bs.new]
external memCache: options => memCache = "default";

let createMemCache = (~size=?, ~ttl=?, ~initialState=?, ()) => {
  memCache(options(~size?, ~ttl?, ~initialState?, ()));
};