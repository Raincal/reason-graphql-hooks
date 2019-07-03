open GraphqlHooksTypes;

type variant('a) =
  | Data('a)
  | Error(error)
  | Loading
  | NoData;

type result('a) = {
  data: option('a),
  loading: bool,
  cacheHit: bool,
  error: bool,
  graphQLErrors: option(graphqlErrors),
  httpError: option(httpError),
  fetchError: option(fetchError),
};

module Make = (Config: Config) => {
  [@bs.deriving abstract]
  type options = {
    [@bs.optional]
    useCache: bool,
    [@bs.optional]
    isManual: bool,
    [@bs.optional]
    variables: Js.Json.t,
    [@bs.optional]
    operationName: string,
    [@bs.optional]
    skipCache: bool,
    [@bs.optional]
    ssr: bool,
    [@bs.optional]
    fetchOptionsOverrides: Fetch.requestInit,
    [@bs.optional]
    updateData: (Config.t, Config.t) => Config.t,
  };

  type jsResult = {
    .
    "loading": bool,
    "cacheHit": bool,
    "data": Js.Nullable.t(Js.Json.t),
    "error": bool,
    "graphQLErrors": Js.Nullable.t(graphqlErrors),
    "fetchError": Js.Nullable.t(fetchError),
    "httpError": Js.Nullable.t(httpError),
    [@bs.meth] "refetch": options => Js.Promise.t(jsResult),
  };

  [@bs.module "graphql-hooks"]
  external useQuery: (string, options) => jsResult = "";

  let use =
      (
        ~useCache=?,
        ~isManual=?,
        ~variables=?,
        ~operationName=?,
        ~skipCache=?,
        ~ssr=?,
        ~updateData=?,
        (),
      ) => {
    let jsResult =
      useQuery(
        Config.query,
        options(
          ~useCache?,
          ~isManual?,
          ~variables?,
          ~operationName?,
          ~skipCache?,
          ~ssr?,
          ~updateData?,
          (),
        ),
      );

    let result = {
      data:
        jsResult##data
        ->Js.Nullable.toOption
        ->Belt.Option.flatMap(data =>
            switch (Config.parse(data)) {
            | parsedData => Some(parsedData)
            | exception _ => None
            }
          ),
      loading: jsResult##loading,
      cacheHit: jsResult##cacheHit,
      error: jsResult##error,
      graphQLErrors: jsResult##graphQLErrors->Js.Nullable.toOption,
      httpError: jsResult##httpError->Js.Nullable.toOption,
      fetchError: jsResult##fetchError->Js.Nullable.toOption,
    };

    let refetch = (~variables=?, ()) =>
      jsResult##refetch(options(~variables?, ()));

    (
      switch (result) {
      | {loading: true, data: None} => Loading
      | {data: Some(data)} => Data(data)
      | {error: true, graphQLErrors: Some(errors)} =>
        Error(GraphQLErrors(errors))
      | {error: true, httpError: Some(error)} => Error(HttpError(error))
      | {error: true, fetchError: Some(error)} => Error(FetchError(error))
      | _ => NoData
      },
      result,
      refetch,
    );
  };
};