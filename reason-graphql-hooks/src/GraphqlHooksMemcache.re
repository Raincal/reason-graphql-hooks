open GraphqlHooksTypes;

type lru;
type cacheData;
type options;

[@bs.deriving abstract]
type cacheKeyObject = {
  operation,
  fetchOptions: options,
};

type memCache = {
  .
  [@bs.meth] "get": cacheKeyObject => cacheData,
  [@bs.meth] "set": (cacheKeyObject, cacheData) => unit,
  [@bs.meth] "delete": cacheKeyObject => unit,
  [@bs.meth] "clear": unit => lru,
  [@bs.meth] "getInitialState": unit => Js.Json.t,
  [@bs.meth] "keys": unit => array(string),
};

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