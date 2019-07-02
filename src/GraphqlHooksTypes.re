module type Config = {
  type t;
  let query: string;
  let parse: Js.Json.t => t;
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