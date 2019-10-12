module type Config = {
  type t;
  let query: string;
  let parse: Js.Json.t => t;
};

[@bs.deriving abstract]
type operation = {
  query: string,
  variables: Js.Nullable.t(Js.Json.t),
  operationName: Js.Nullable.t(string),
};

type graphqlError = {
  .
  "message": string,
  "name": Js.Nullable.t(string),
  "locations": Js.Nullable.t(array(string)),
  "path": Js.Nullable.t(array(string)),
  "nodes": Js.Nullable.t(array(string)),
};

type graphqlErrors = array(graphqlError);

type httpError = {
  .
  "status": int,
  "statusText": string,
  "body": string,
};

type fetchError;

type error =
  | GraphQLErrors(graphqlErrors)
  | HttpError(httpError)
  | FetchError(fetchError);

type response('a) =
  | Data('a)
  | Error(error)
  | Loading
  | NoData;

type result('a) = {
  data: option('a),
  loading: bool,
  error: bool,
  graphQLErrors: option(graphqlErrors),
  httpError: option(httpError),
  fetchError: option(fetchError),
};

type hooksResponse('a) = {
  loading: bool,
  data: option('a),
  cacheHit: bool,
  response: response('a),
};
