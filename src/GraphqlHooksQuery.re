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
  type useQueryOptions = {
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

  type useQueryResponseJs = {
    .
    "loading": bool,
    "cacheHit": bool,
    "data": Js.Nullable.t(Js.Json.t),
    "error": bool,
    "graphQLErrors": Js.Nullable.t(graphqlErrors),
    "fetchError": Js.Nullable.t(fetchError),
    "httpError": Js.Nullable.t(httpError),
    [@bs.meth] "refetch": useQueryOptions => Js.Promise.t(useQueryResponseJs),
  };

  [@bs.module "graphql-hooks"]
  external useQueryJs: (string, useQueryOptions) => useQueryResponseJs =
    "useQuery";

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
      useQueryJs(
        Config.query,
        useQueryOptions(
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
        jsResult##data->Js.Nullable.toOption->Belt.Option.map(Config.parse),
      loading: jsResult##loading,
      cacheHit: jsResult##cacheHit,
      error: jsResult##error,
      graphQLErrors: jsResult##graphQLErrors->Js.Nullable.toOption,
      httpError: jsResult##httpError->Js.Nullable.toOption,
      fetchError: jsResult##fetchError->Js.Nullable.toOption,
    };

    let refetch = (~variables=?, ()) =>
      jsResult##refetch(useQueryOptions(~variables?, ()));

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