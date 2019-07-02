type operation = {
  .
  "query": string,
  "variables": Js.Json.t,
  "operationName": string,
};

type requestObj = {
  .
  "operation": operation,
  "options": Js.Json.t,
};

type jsResult = {
  .
  "data": Js.Nullable.t(Js.Json.t),
  "error": bool,
};

type generatedGraphQLClient = {
  .
  "request": [@bs.meth] (requestObj => Js.Promise.t(jsResult)),
};

[@bs.deriving abstract]
type options = {
  url: string,
  [@bs.optional]
  ssrMode: bool,
  [@bs.optional]
  cache: GraphqlHooksMemcache.memCache,
  [@bs.optional]
  logErrors: bool,
};

[@bs.module "graphql-hooks"] [@bs.new]
external createGraphQLClientJS: options => generatedGraphQLClient =
  "GraphQLClient";

let createGraphQLClient = (~url, ~ssrMode=?, ~cache=?, ~logErrors=?, ()) => {
  createGraphQLClientJS(options(~url, ~ssrMode?, ~cache?, ~logErrors?, ()));
};