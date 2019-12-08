type hooksResponse('ret) =
  GraphqlHooksTypes.hooksResponse('ret) = {
    loading: bool,
    data: option('ret),
    cacheHit: bool,
    response: GraphqlHooksTypes.response('ret),
  };

[@bs.deriving abstract]
type useMutationOptions = {
  [@bs.optional]
  variables: Js.Json.t,
  [@bs.optional]
  operationName: string,
  [@bs.optional]
  fetchOptionsOverrides: Fetch.requestInit,
};

type useMutationResponseJs = {
  .
  "loading": bool,
  "cacheHit": bool,
  "data": Js.Nullable.t(Js.Json.t),
  "error": option(GraphqlHooksTypes.combinedErrorJs),
};

type executeMutation =
  useMutationOptions => Js.Promise.t(useMutationResponseJs);

[@bs.module "graphql-hooks"]
external useMutationJs:
  (string, useMutationOptions) => (executeMutation, useMutationResponseJs) =
  "useMutation";

let useMutation = (~request, ~operationName=?, ()) => {
  let (executeMutation, useMutationResponseJs) =
    useMutationJs(
      request##query,
      useMutationOptions(~variables=request##variables, ~operationName?, ()),
    );

  let useMutationResponseToReason = (parse, state) => {
    let data = state##data->Js.Nullable.toOption->Belt.Option.map(parse);
    let loading = state##loading;
    let error =
      state##error->Belt.Option.map(GraphqlHooksTypes.combinedErrorToRecord);
    let cacheHit = state##cacheHit;

    let result: GraphqlHooksTypes.result('data) = {data, loading, error};

    let response: GraphqlHooksTypes.response('data) =
      switch (result) {
      | {loading: true} => Loading
      | {error: Some(error)} =>
        switch (error) {
        | {graphQLErrors: Some(errors)} => Error(GraphQLErrors(errors))
        | {httpError: Some(error)} => Error(HttpError(error))
        | {fetchError: Some(error)} => Error(FetchError(error))
        | _ => NoData
        }
      | {data: Some(data)} => Data(data)
      | _ => NoData
      };

    {loading, data, cacheHit, response};
  };

  let useMutationResponse =
    React.useMemo2(
      () =>
        useMutationResponseJs |> useMutationResponseToReason(request##parse),
      (request##parse, useMutationResponseJs),
    );

  let executeMutation =
    React.useMemo1(
      ((), ~variables=?, ()) =>
        executeMutation(useMutationOptions(~variables?, ())),
      [|request##variables|],
    );

  (useMutationResponse, executeMutation);
};