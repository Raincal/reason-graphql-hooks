type t;

[@bs.deriving abstract]
type clientOptions = {
  url: string,
  [@bs.optional]
  ssrMode: bool,
  [@bs.optional]
  cache: GraphqlHooksMemcache.memCache,
  [@bs.optional]
  logErrors: bool,
};

[@bs.module "graphql-hooks"] [@bs.new]
external client: clientOptions => t = "GraphQLClient";

let make = (~url, ~ssrMode=?, ~cache=?, ~logErrors=?, ()) => {
  let options = clientOptions(~url, ~ssrMode?, ~cache?, ~logErrors?, ());
  client(options);
};